/* ===========================================================================
** Copyright (C) 2022 Infineon Technologies AG
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

#include "argparse.h"
#include "DaqKitConverter.hpp"

int main(int argc, char *argv[])
{
    char *record_data_file = nullptr;
    char *config_json_file = nullptr;
    char *output_path = nullptr;

    const char *const usage_str[] = {
        "DaqKit converter [OPTIONS] [FILE]",
        nullptr};

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Options"),
        OPT_STRING('d', "data", &record_data_file, "Select the data file recorded by app_recorder.", nullptr, 0, 0),
        OPT_STRING('c', "config", &config_json_file, "Select the config.json file created by app_recorder.", nullptr, 0, 0),
        OPT_STRING('p', "path", &output_path, "Select output directory [optional].", nullptr, 0, 0),
        OPT_GROUP("Examples:\n\
    daqkit_converter -d [FILE] -c [FILE] -p[PATH] Save row data file in DaqKit format to the selected path."),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usage_str, 0);
    argparse_describe(&argparse, "Tool for converting Radar records to DaqKit format.", nullptr);
    argparse_parse(&argparse, argc, argv);

    Infineon::Converter::DaqKitConverter converter;

	if (!record_data_file)
	{
		std::cerr << "Input file is missing\n";
		return 1;
	}

	if (!config_json_file)
	{
		std::cerr << "Configuration file is missing\n";
		return 1;
	}

	if (!output_path)
	{
		std::cerr << "Output path is missing\n";
		return 1;
	}

	/**
	* Convert raw data file to DaqKit format created by app_recorder.
	* Example: ./daqkit -d recording -c recording_config.json -p /home/user/records
	* Output:
	* /home/user/records
						│
						└───RadarIfxAvian_xx
								format.version
								meta.json
								radar.npy
								recording_config.json
	*/

	try
	{
		converter.loadFromFiles(config_json_file, record_data_file, output_path);
		converter.saveToDaqKitFormat();
		std::cout << "DaqKit files have been saved.\n";
	}
	catch (const Infineon::Converter::DaqKitException& e)
	{
		std::cout << "DaqKitException : " << e.what() << '\n';
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception : " << e.what() << '\n';
	}
	catch (...)
	{
		std::cout << "Unknown error\n";
	}


    return 0;
}
