/*
* (c) Copyright, Real-Time Innovations, 2020.  All rights reserved.
* RTI grants Licensee a license to use, modify, compile, and create derivative
* works of the software solely for use with RTI Connext DDS. Licensee may
* redistribute copies of the software provided that all such copies are subject
* to this license. The software is provided "as is", with no warranty of any
* type, including any warranty for fitness for any purpose. RTI is under no
* obligation to maintain or support the software. RTI shall not be liable for
* any incidental or consequential damages arising out of the use or inability
* to use the software.
*/

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <iostream>
#include <csignal>
#include <dds/core/ddscore.hpp>

namespace application {

    // Catch control-C and tell application to shut down
    bool shutdown_requested = false;

    inline void stop_handler(int)
    {
        shutdown_requested = true;
        std::cout << "preparing to shut down..." << std::endl;
    }

    inline void setup_signal_handlers()
    {
        signal(SIGINT, stop_handler);
        signal(SIGTERM, stop_handler);
    }

    enum class ParseReturn {
        ok,
        failure,
        exit
    };

    struct ApplicationArguments {
        ParseReturn parse_result;
        unsigned domain_id;
        std::string server_host;
        std::string server_port;
        std::string participation_password;
        rti::config::Verbosity verbosity;

        ApplicationArguments(
            ParseReturn parse_result_param,
            unsigned domain_id,
            std::string server_host,
            std::string server_port,
            std::string participation_password,
            rti::config::Verbosity verbosity_param
        ) : parse_result(parse_result_param),
            domain_id(domain_id),
            server_host(server_host),
            server_port(server_port),
            participation_password(participation_password),
            verbosity(verbosity_param)
        {}
    };

    inline void set_verbosity(
        rti::config::Verbosity& verbosity,
        int verbosity_value)
    {
        switch (verbosity_value) {
            case 0:
            verbosity = rti::config::Verbosity::SILENT;
            break;
            case 1:
            verbosity = rti::config::Verbosity::EXCEPTION;
            break;
            case 2:
            verbosity = rti::config::Verbosity::WARNING;
            break;
            case 3:
            verbosity = rti::config::Verbosity::STATUS_ALL;
            break;
            default:
            verbosity = rti::config::Verbosity::EXCEPTION;
            break;
        }
    }

    // Parses application arguments for example.
    inline ApplicationArguments parse_arguments(int argc, char *argv[])
    {
        int arg_processing = 1;
        bool show_usage = false;

        ParseReturn parse_result = ParseReturn::ok;
        unsigned domain_id = 0;
        std::string server_host = "127.0.0.1";
        std::string server_port = "38866";
        std::string participation_password = "f3d3r4t3";
        rti::config::Verbosity verbosity(rti::config::Verbosity::EXCEPTION);

        while (arg_processing < argc)
            if ((argc > arg_processing + 1) 
            && (strcmp(argv[arg_processing], "-d") == 0
            || strcmp(argv[arg_processing], "--domain") == 0)) {
                domain_id = atoi(argv[arg_processing + 1]);
                arg_processing += 2;
            } else if ((argc > arg_processing + 1)
            && (strcmp(argv[arg_processing], "-n") == 0
            || strcmp(argv[arg_processing], "--host") == 0)) {
                server_host = argv[arg_processing + 1];
                arg_processing += 2;
            } else if ((argc > arg_processing + 1)
            && (strcmp(argv[arg_processing], "-p") == 0
            || strcmp(argv[arg_processing], "--port") == 0)) {
                server_port = argv[arg_processing + 1];
                arg_processing += 2;
            } else if ((argc > arg_processing + 1)
            && strcmp(argv[arg_processing], "--password") == 0) {
                participation_password = argv[arg_processing + 1];
                arg_processing += 2;
            } else if ((argc > arg_processing + 1)
            && (strcmp(argv[arg_processing], "-v") == 0
            || strcmp(argv[arg_processing], "--verbosity") == 0)) {
                set_verbosity(verbosity, atoi(argv[arg_processing + 1]));
                arg_processing += 2;
            } else if (strcmp(argv[arg_processing], "-h") == 0
            || strcmp(argv[arg_processing], "--help") == 0) {
                show_usage = true;
                parse_result = ParseReturn::exit;
                break;
            } else {
                std::cerr << "unknown argument supplied" << std::endl;
                show_usage = true;
                parse_result = ParseReturn::failure;
                break;
            }

        if (show_usage)
            std::cout << "usage:\n" \
            "    -d, --domain        domain ID to communicate over\n" \
            "                               default: 0" \
            "    -n, --host          hostname of the server's microservice\n" \
            "                               default: 127.0.0.1" \
            "    -p, --port          port of the server's microservice\n" \
            "                               default: 38866" \
            "        --password      password that grants access to the network\n" \
            "                               default: f3d3r4t3" \
            "    -v, --verbosity     debugging verbosity\n" \
            "                               range: 0-3 \n" \
            "                               default: 1"
            << std::endl;

        return ApplicationArguments(
            parse_result,
            domain_id,
            server_host,
            server_port,
            participation_password,
            verbosity
        );
    }

}  // namespace application

#endif  // APPLICATION_HPP
