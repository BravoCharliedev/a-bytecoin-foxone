
#include "BlockManager.h"

#include <ctime>
#include <fstream>
#include <future>
#include <iomanip>
#include <thread>
#include <set>
#include <sstream>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "Common/CommandLine.h"
#include "Common/SignalHandler.h"
#include "Common/StringTools.h"
#include "Common/PathTools.h" /// 
#include "Common/Util.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "CryptoNoteProtocol/CryptoNoteProtocolHandler.h"
#include "NodeRpcProxy/NodeRpcProxy.h"
#include "Rpc/CoreRpcServerCommandsDefinitions.h"
#include "Rpc/HttpClient.h"
#include "Rpc/JsonRpc.h"

#include "version.h"

#include <Logging/LoggerManager.h>

//#include "Wallet/WalletRpcServer.h"
using namespace CryptoNote;
using namespace Logging;
using Common::JsonValue;

namespace po = boost::program_options;

namespace {

	const command_line::arg_descriptor<std::string> arg_daemon_address = { "daemon-address", "Use daemon instance at <host>:<port>", "" };
	const command_line::arg_descriptor<std::string> arg_daemon_host = { "daemon-host", "Use daemon instance at host <arg> instead of localhost", "" };
	const command_line::arg_descriptor<uint16_t> arg_daemon_port = { "daemon-port", "Use daemon instance at port <arg> instead of 29971", 0 };
	const command_line::arg_descriptor<uint32_t> arg_log_level = { "set_log", "", INFO, true };
	const command_line::arg_descriptor<bool> arg_testnet = { "testnet", "Used to deploy test nets. The daemon must be launched with --testnet flag", false };
	const command_line::arg_descriptor< std::vector<std::string> > arg_command = { "command", "" };


	bool parseUrlAddress(const std::string& url, std::string& address, uint16_t& port) {
		auto pos = url.find("://");
		size_t addrStart = 0;

		if (pos != std::string::npos) {
			addrStart = pos + 3;
		}

		auto addrEnd = url.find(':', addrStart);

		if (addrEnd != std::string::npos) {
			auto portEnd = url.find('/', addrEnd);
			port = Common::fromString<uint16_t>(url.substr(
				addrEnd + 1, portEnd == std::string::npos ? std::string::npos : portEnd - addrEnd - 1));
		}
		else {
			addrEnd = url.find('/');
			port = 80;
		}

		address = url.substr(addrStart, addrEnd - addrStart);
		return true;
	}

	inline std::string interpret_rpc_response(bool ok, const std::string& status) {
		std::string err;
		if (ok) {
			if (status == CORE_RPC_STATUS_BUSY) {
				err = "daemon is busy. Please try later";
			}
			else if (status != CORE_RPC_STATUS_OK) {
				err = status;
			}
		}
		else {
			err = "possible lost connection to daemon";
		}
		return err;
	}

	JsonValue buildLoggerConfiguration(Level level, const std::string& logfile) {
		JsonValue loggerConfiguration(JsonValue::OBJECT);
		loggerConfiguration.insert("globalLevel", static_cast<int64_t>(level));

		JsonValue& cfgLoggers = loggerConfiguration.insert("loggers", JsonValue::ARRAY);

		JsonValue& consoleLogger = cfgLoggers.pushBack(JsonValue::OBJECT);
		consoleLogger.insert("type", "console");
		consoleLogger.insert("level", static_cast<int64_t>(TRACE));
		consoleLogger.insert("pattern", "%D %T %L ");

		JsonValue& fileLogger = cfgLoggers.pushBack(JsonValue::OBJECT);
		fileLogger.insert("type", "file");
		fileLogger.insert("filename", logfile);
		fileLogger.insert("level", static_cast<int64_t>(TRACE));

		return loggerConfiguration;
	}

}

std::string BlockManager::get_commands_str() {
	std::stringstream ss;
	ss << "Commands: " << ENDL;
	std::string usage = m_consoleHandler.getUsage();
	boost::replace_all(usage, "\n", "\n  ");
	usage.insert(0, "  ");
	ss << usage << ENDL;
	return ss.str();
}

bool BlockManager::help(const std::vector<std::string> &args) {
	success_msg_writer() << get_commands_str();
	return true;
}

bool BlockManager::exit(const std::vector<std::string> &args) {
	m_consoleHandler.requestStop();
	return true;
}

BlockManager::BlockManager(System::Dispatcher& dispatcher, Logging::LoggerManager& log) :
	m_dispatcher(dispatcher),
	m_daemon_port(0),
	logManager(log),
	logger(log, "BlockManager")
{
	m_consoleHandler.setHandler("help", boost::bind(&BlockManager::help, this, _1), "Show this help");
	m_consoleHandler.setHandler("set_log", boost::bind(&BlockManager::set_log, this, _1), "set_log <level> - Change current log level, <level> is a number 0-4");
	m_consoleHandler.setHandler("bc_height", boost::bind(&BlockManager::show_blockchain_height, this, _1), "Show blockchain height");
	m_consoleHandler.setHandler("push_block", boost::bind(&BlockManager::push_block, this, _1), "Push block Whit (< Account Number>  < Public Address>)");
	//m_consoleHandler.setHandler("print_list", boost::bind(&BlockManager::print_list, this, _1), "print list of all accoun and address");
	m_consoleHandler.setHandler("find", boost::bind(&BlockManager::find_address, this, _1), "find address and hash < Account Number>  ");
	//m_consoleHandler.setHandler("print_hash",boost::bind(&BlockManager::print_hash, this, _1), "print hash"); 
	m_consoleHandler.setHandler("exit", boost::bind(&BlockManager::exit, this, _1), "Close Block Manager");
}

void BlockManager::handle_command_line(const boost::program_options::variables_map& vm) {

	m_daemon_address = command_line::get_arg(vm, arg_daemon_address);
	m_daemon_host = command_line::get_arg(vm, arg_daemon_host);
	m_daemon_port = command_line::get_arg(vm, arg_daemon_port);

}

bool BlockManager::set_log(const std::vector<std::string> &args) {
	if (args.size() != 1) {
		fail_msg_writer() << "use: set_log <log_level_number_0-4>";
		return true;
	}

	uint16_t l = 0;
	if (!Common::fromString(args[0], l)) {
		fail_msg_writer() << "wrong number format, use: set_log <log_level_number_0-4>";
		return true;
	}

	if (l > Logging::TRACE) {
		fail_msg_writer() << "wrong number range, use: set_log <log_level_number_0-4>";
		return true;
	}

	logManager.setMaxLevel(static_cast<Logging::Level>(l));
	return true;
}



bool BlockManager::init(const boost::program_options::variables_map& vm) {
	handle_command_line(vm);

	if (!m_daemon_address.empty() && (!m_daemon_host.empty() || 0 != m_daemon_port)) {
		fail_msg_writer() << "you can't specify daemon host or port several times";
		return false;
	}

	if (m_daemon_host.empty())
		m_daemon_host = "localhost";
	if (!m_daemon_port)
		m_daemon_port = RPC_DEFAULT_PORT;

	if (!m_daemon_address.empty()) {
		if (!parseUrlAddress(m_daemon_address, m_daemon_host, m_daemon_port)) {
			fail_msg_writer() << "failed to parse daemon address: " << m_daemon_address;
			return false;
		}
	}
	else {
		m_daemon_address = std::string("http://") + m_daemon_host + ":" + std::to_string(m_daemon_port);
	}

	this->m_node.reset(new NodeRpcProxy(m_daemon_host, m_daemon_port, logger.getLogger()));

	std::promise<std::error_code> errorPromise;
	std::future<std::error_code> f_error = errorPromise.get_future();
	auto callback = [&errorPromise](std::error_code e) {errorPromise.set_value(e); };

	m_node->addObserver(static_cast<INodeRpcProxyObserver*>(this));
	m_node->init(callback);
	auto error = f_error.get();
	if (error) {
		fail_msg_writer() << "failed to init NodeRPCProxy: " << error.message();
		return false;
	}

	else {
		m_node->addObserver(static_cast<INodeObserver*>(this));

		logger(INFO, BRIGHT_WHITE) <<ENDL<< "Conecction Sucessfull " << ENDL;

		success_msg_writer() <<
			"\n**********************************************************************\n" <<
			"Use \"help\" command to see the list of available commands.\n" <<
			"**********************************************************************\n";
		logger(INFO, BRIGHT_YELLOW)<<"Do not \"push_block\" in the same node with a Miner working\n";

	}

	return true;
}

bool BlockManager::deinit() {
	m_node->removeObserver(static_cast<INodeObserver*>(this));
	m_node->removeObserver(static_cast<INodeRpcProxyObserver*>(this));

	return true;
}

void BlockManager::connectionStatusUpdated(bool connected) {
	if (connected) {
		logger(INFO, GREEN) << "Block Manager connected to daemon.";
	}
	else {
		printConnectionError();
	}
}

bool BlockManager::show_blockchain_height(const std::vector<std::string>& args) {
	try {
		uint64_t bc_height = m_node->getLastLocalBlockHeight();
		success_msg_writer() << bc_height;
	}
	catch (std::exception &e) {
		logger(INFO, RED) << "failed to get blockchain height: " << e.what();
	}

	return true;
}

bool BlockManager::run() {
	std::cout << std::endl;

	m_consoleHandler.start(false, "Block Manager started \n", Common::Console::Color::BrightYellow);
	return true;
}

void BlockManager::stop() {
	m_consoleHandler.requestStop();
}

bool BlockManager::process_command(const std::vector<std::string> &args) {
	return m_consoleHandler.runCommand(args);
}

bool BlockManager::push_block(const std::vector<std::string> &args) {
	if (args.empty() || args.size()<2) {
		logger(INFO, WHITE) << "expected: print_block (< Account Number>  < Public Address>)" << std::endl;
		return true;
	}
	if (args[0].size() != 10) {
		logger(INFO, YELLOW) << "invalid Accoun Number)" << std::endl;
		return true;
	}
	if (args[1].size() != 95) {
		logger(INFO, YELLOW) << "invalid Public Address)" << std::endl;
		return true;
	}


	COMMAND_RPC_PUSHBLOCK::request req;
	COMMAND_RPC_PUSHBLOCK::response res;
	req.AccountAddress = args[1];
	req.AccountNumber = args[0];

	try {
		HttpClient httpClient(m_dispatcher, m_daemon_host, m_daemon_port);

		JsonRpc::invokeJsonRpcCommand(httpClient, "pushblock", req, res);
	}
	catch (const ConnectException&) {
		printConnectionError();
	}
	catch (const std::exception& e) {
		fail_msg_writer() << "Failed to invoke rpc method: " << e.what();
		return false;
	}

	logger(INFO, WHITE) << res.status << ENDL;
	return true;
}

bool CryptoNote::BlockManager::find_address(const std::vector<std::string>& args)
{
	if (args[0].size() != 10) {
		logger(INFO, WHITE) << "invalid addres .... " << ENDL;
		return true;
	}

	if (args[0].size() == 10) {
		COMMAND_RPC_GETBLOCKADDRESS::request req;
		COMMAND_RPC_GETBLOCKADDRESS::response res;
		req.AccountNumber = args[0];
		try {
			HttpClient httpClient(m_dispatcher, m_daemon_host, m_daemon_port);

			invokeJsonCommand(httpClient, "/getblockaddress", req, res);
		}
		catch (const ConnectException&) {
			printConnectionError();
		}
		catch (const std::exception& e) {
			fail_msg_writer() << "Failed to invoke rpc method: " << e.what();
		}

		logger(INFO, BRIGHT_WHITE)<< "The accoun number:" << args[0] << " is associated whit"<<ENDL<< "Address: " << res.AccountAddress << ENDL<< "HASH: "<<res.hash<< ENDL;
	}

	return false;
}


///////////////////////////////////// falta ////////////////////////////////////////////

bool CryptoNote::BlockManager::print_list(const std::vector<std::string>& args)
{
	return false;
}// falta programar 

bool CryptoNote::BlockManager::print_hash(const std::vector<std::string>& args){
	if (args[0].size() != 10) {
		logger(INFO, WHITE) << "invalid addres .... " << ENDL;
		return true;
	}

	if (args[0].size() == 10) {
		COMMAND_RPC_GETBLOCKHASH_BM::request req;
		COMMAND_RPC_GETBLOCKHASH_BM::response res;
		req.AccountNumber = args[0];
		try {
			HttpClient httpClient(m_dispatcher, m_daemon_host, m_daemon_port);

			invokeJsonCommand(httpClient, "/getblockhash", req, res);
		}
		catch (const ConnectException&) {
			printConnectionError();
		}
		catch (const std::exception& e) {
			fail_msg_writer() << "Failed to invoke rpc method: " << e.what();
		}
		logger(INFO, BRIGHT_WHITE) << res.Status << ENDL;
		logger(INFO, BRIGHT_WHITE) << "The accoun number:" << args[0] << " is associated whit the hash : " << res.hash << ENDL;
	}

	return false;
} //falta programar 

///////////////////////////////////////////////////////////////////////////////////////////

void BlockManager::printConnectionError() const {
	fail_msg_writer() << "failed to connect to daemon (" << m_daemon_address << ").";
}


int main(int argc, char* argv[]) {
#ifdef WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	po::options_description desc_general("General options");
	command_line::add_arg(desc_general, command_line::arg_help);
	command_line::add_arg(desc_general, command_line::arg_version);

	po::options_description desc_params("Block Manager options");
	command_line::add_arg(desc_params, arg_daemon_address);
	command_line::add_arg(desc_params, arg_daemon_host);
	command_line::add_arg(desc_params, arg_daemon_port);
	command_line::add_arg(desc_params, arg_command);
	command_line::add_arg(desc_params, arg_log_level);
	command_line::add_arg(desc_params, arg_testnet);


	po::positional_options_description positional_options;
	positional_options.add(arg_command.name, -1);

	po::options_description desc_all;
	desc_all.add(desc_general).add(desc_params);

	Logging::LoggerManager logManager;
	Logging::LoggerRef logger(logManager, "BlockManager");
	System::Dispatcher dispatcher;

	po::variables_map vm;
	
	bool r = command_line::handle_error_helper(desc_all, [&]() {
		po::store(command_line::parse_command_line(argc, argv, desc_general, true), vm);

		if (command_line::get_arg(vm, command_line::arg_help)) {
			CryptoNote::BlockManager tmp_blockManager(dispatcher, logManager);

			std::cout << CRYPTONOTE_NAME << " Block Manager v" << PROJECT_VERSION_LONG << std::endl;
		}
		else if (command_line::get_arg(vm, command_line::arg_version)) {
			std::cout << CRYPTONOTE_NAME << " wallet v" << PROJECT_VERSION_LONG;
			return false;
		}

		auto parser = po::command_line_parser(argc, argv).options(desc_params).positional(positional_options);
		po::store(parser.run(), vm);
		po::notify(vm);
		return true;
	});

	if (!r)
		return 1;
	//set up logging options
	
	Level logLevel = DEBUGGING;

	if (command_line::has_arg(vm, arg_log_level)) {
		logLevel = static_cast<Level>(command_line::get_arg(vm, arg_log_level));
	}

	logManager.configure(buildLoggerConfiguration(logLevel, Common::ReplaceExtenstion(argv[0], ".log")));

	logger(INFO, BRIGHT_WHITE) << CRYPTONOTE_NAME << " Block Manager v" << PROJECT_VERSION_LONG;

	//runs BlockManager with console interface
	CryptoNote::BlockManager wal(dispatcher, logManager);
	if (!wal.init(vm)) {
		logger(ERROR, BRIGHT_RED) << "Failed to initialize Block Manager";
		return 1;
	}
	std::vector<std::string> command = command_line::get_arg(vm, arg_command);
	if (!command.empty())
		wal.process_command(command);
	Tools::SignalHandler::install([&wal] {
		wal.stop();
	});
	wal.run();
	if (!wal.deinit()) {
		logger(ERROR, BRIGHT_RED) << "Failed to close Block Manager";
	}
	else {
		logger(INFO) << "Block Manager closed";

	}
	return 1;
}


