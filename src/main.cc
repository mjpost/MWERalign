/*--------------------------------------------------------------
 Copyright 2006 (c) by RWTH Aachen - Lehrstuhl fuer Informatik 6
 Richard Zens, Evgeny Matusov, Gregor Leusch
 ---------------------------------------------------------------

This header file is part of the MwerAlign C++ Library.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.
*/
#include "mwerAlign.hh"
#include <fstream>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

struct option_plus {
    const char *name;
    int has_arg;
    int *flag;
    int val;
    const char *desc;
};

static vector<option_plus> options_plus = {
    {"help", no_argument, 0, 'h', "print this help message"},
    {"hyp", required_argument, 0, 't', "hypothesis side of the text to align (ideally, one doc per line)"},
    {"ref", required_argument, 0, 'r', "reference side of the text to align"},
    {"docids", required_argument, 0, 'd', "document ids (parallel to ref)"},
    {"out", required_argument, 0, 'o', "output file"},
    {0, 0, 0, 0, 0}};

void print_usage()
{
    std::cout << "Options:" << std::endl;
    for (auto it = options_plus.begin(); it != options_plus.end(); ++it) {
        if (it->name == 0)
            continue;
        string name(it->name);
        if (it->has_arg == required_argument)
            name += " arg";
        std::cout << std::setw(26) << std::left << "  --" + name << " " << it->desc << std::endl;
    }
    std::cout << std::endl;
}

/* Split a string on a delimiter */
std::vector<std::string> split(const std::string &str, const std::string &delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0, end;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }
    tokens.push_back(str.substr(start));

    return tokens;
}

int main(int argc, char *argv[])
{
    string hyp_file;
    string ref_file;
    string doc_file;
    string out_file = "/dev/stdout";
    vector<option> options;
    for (auto it = options_plus.begin(); it != options_plus.end(); ++it) {
        option op = {it->name, it->has_arg, it->flag, it->val};
        options.push_back(op);
    }
    // create a string of short options as a char* and assign value "hs:t:o:m:"
    const char *short_options = "ht:r:d:o:m:";

    optind = 0; // necessary to parse arguments twice
    while (1) {
        int option_index = 0;
        int opt = getopt_long(argc, argv, short_options, options.data(), &option_index);
        if (opt == -1)
            break;

        switch (opt) {
        case 0:
            break;
        case 'h':
            print_usage();
            return 0;
        case 't':
            hyp_file = string(optarg);
            break;
        case 'r':
            ref_file = string(optarg);
            break;
        case 'd':
            doc_file = string(optarg);
            break;
        case 'o':
            out_file = string(optarg);
            break;
        default:
            abort();
        }
    }

    if (hyp_file.empty() || ref_file.empty()) {
        print_usage();
        return 0;
    }

    std::cerr << "hypothesis file: " << hyp_file << std::endl;
    std::cerr << "reference file: " << ref_file << std::endl;
    std::cerr << "document ids file: " << doc_file << std::endl;
    std::cerr << "output file: " << out_file << std::endl;

    // You can also load a serialized model from std::string.
    // const std::string str = // Load blob contents from a file.
    // auto status = processor.LoadFromSerializedProto(str);

    ifstream hypFile(hyp_file);
    vector<string> hyp_data;
    if (hypFile) {
        string line = "";
        while (getline(hypFile, line)) {
            hyp_data.push_back(line);
        }
    } else {
        std::cerr << "Error: Could not open hypothesis file " << hyp_file << std::endl;
        return 1;
    }

    vector<string> ref_data;
    ifstream refFile(ref_file);
    if (refFile) {
        string line = "";
        while (std::getline(refFile, line)) {
            ref_data.push_back(line);
        }
    } else {
        std::cerr << "Error: Could not open reference file " << ref_file << std::endl;
        return 1;
    }

    vector<string> doc_data;
    vector<string> doc_id_list;
    ifstream docFile(doc_file);
    if (docFile) {
        string line = "";
        while (std::getline(docFile, line)) {
            doc_data.push_back(line);
            if (doc_id_list.size() == 0 or doc_id_list.back() != line) {
                doc_id_list.push_back(line);
            }
        }

        if (doc_data.size() != ref_data.size()) {
            std::cerr << "Error: Number of document ids (" << doc_data.size()
                      << ") does not match number of references (" << ref_data.size() << ")" << std::endl;
            return 1;
        }

        // make sure the number of distinct docids matches the number of hypotheses
        std::set<string> doc_ids_set(doc_data.begin(), doc_data.end());
        if (doc_ids_set.size() != hyp_data.size()) {
            std::cerr << "Error: Number of distinct document ids (" << doc_ids_set.size()
                      << ") does not match number of hypotheses (" << hyp_data.size() << ")" << std::endl;
            return 1;
        }
    } else {
        // create a fake doc_data dataset with the same entry, "0", for every line of ref_data
        // do this using a utlity function
        string dummy_doc_id = "0";
        for (size_t i = 0; i < ref_data.size(); ++i) {
            doc_data.push_back(dummy_doc_id);
        }
        doc_id_list.push_back(dummy_doc_id);

        // and move all the hypothesis data to a single entry
        for (size_t i = 1; i < hyp_data.size(); ++i) {
            hyp_data[0] += " " + hyp_data[i];
        }
        hyp_data.resize(1);
    }

    /* Align each document separately.
       This is done by iterating through the doc IDs. For each doc ID, collect all the references and concatenate them
       into a string, joined by "\n". Then get the corresponding line number from the hypothesis file. Then call
       ms.mwerAlign() on the two strings.
     */
    MwerSegmenter ms;
    ms.setsegmenting(false);
    string result;

    // iterate through the doc_data
    for (size_t i = 0; i < doc_id_list.size(); ++i) {
        // get the current doc id
        string doc_id = doc_id_list[i];
        // get the corresponding line number from the hypothesis file
        string hyp_line = hyp_data[i];
        // collect all the references with the same doc id
        string ref_lines;
        int num_ref_lines = 0;
        for (size_t j = 0; j < doc_data.size(); ++j) {
            if (doc_data[j] == doc_id) {
                ref_lines += ref_data[j] + "\n";
                num_ref_lines++;
            }
        }
        // remove the last newline
        ref_lines = ref_lines.substr(0, ref_lines.size() - 1);

        size_t num_hyp_words = std::count(hyp_line.begin(), hyp_line.end(), ' ') + 1;
        if (doc_id_list.size() > 1)
            std::cerr << "Aligning doc id " << doc_id << " (" << i + 1 << "/" << doc_id_list.size() << ": ";
        else
            std::cerr << "Aligning ";
        std::cerr << num_hyp_words << " words to " << num_ref_lines << " references" << std::endl;

        // call ms.mwerAlign() on the two strings
        ms.mwerAlign(ref_lines, hyp_line, result);
        result += "\n";
    }

    ofstream outFile(out_file);
    if (outFile) {
        result = result.substr(0, result.size() - 1);
        outFile << result << endl;
    }

    return 0;
}