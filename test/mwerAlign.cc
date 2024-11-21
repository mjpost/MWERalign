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
#include <string>
#include "mwerAlign.hh"
#include <string>
#include <fstream>
#include <iostream>
#include <getopt.h>
#include <sstream>
#include <iomanip> 
// 
using namespace std;


struct option_plus {
    const char *name;
    int         has_arg;
    int        *flag;
    int         val;
    const char *desc;
};


static vector<option_plus> options_plus = {
    {"help",          no_argument,       0, 'h', "print this help message"},
    {"hyp",        required_argument, 0, 's', "hypopthesis side of the text to align"},
    {"hyp_bi",        required_argument, 0, 'b', "bitext of the hypopthesis side of the text to align [optional]"},
    {"ref",        required_argument, 0, 't', "reference side of the text to align"},
    {"out",        required_argument, 0, 'o', "output file"},
    {0, 0, 0, 0, 0}
};

void print_usage() {
    std::cout << "Options:" << std::endl;
    for (auto it = options_plus.begin(); it != options_plus.end(); ++it) {
        if (it->name == 0) continue;
        string name(it->name);
        if (it->has_arg == required_argument) name += " arg";
        std::cout << std::setw(26) << std::left << "  --" + name << " " << it->desc << std::endl;
    }
    std::cout << std::endl;
}


int main(int argc,char**argv) {
    string hyp_file;
    string hyp_bi_file;
    string ref_file;
    string out_file;
    vector<option> options;
    for (auto it = options_plus.begin(); it != options_plus.end(); ++it) {
        option op = {it->name, it->has_arg, it->flag, it->val};
        options.push_back(op);
    }

    optind = 0;  // necessary to parse arguments twice
    while (1) {
        int option_index = 0;
        int opt = getopt_long(argc, argv, "hv", options.data(), &option_index);
        if (opt == -1) break;

        switch (opt) {
            case 0:                                         break;
            case 'h': print_usage();                        return 0;
            case 's': hyp_file = string(optarg);            break;
            case 'b': hyp_bi_file = string(optarg);         break;
            case 't': ref_file = string(optarg);            break;
            case 'o': out_file = string(optarg);            break;
            default:                                        abort();
        }
    }

    if (hyp_file.empty() && (ref_file.empty() || out_file.empty())) {
        print_usage();
        return 0;
    }

    string line;
    ifstream hypFile(hyp_file);
    string hyp_data;
    string hyp_bi_data;
    string ref_data;
    if ( hypFile ) 
    {
        hyp_data="";
        line="";
        while ( getline( hypFile, line ) )
        { 
            hyp_data = hyp_data + line + "\n";
        }
    }
    if ((int)hyp_bi_file.size() > 0) 
    {
        ifstream hypBiFile(hyp_bi_file);
        if ( hypBiFile ) 
        {
            hyp_bi_data="";
            line="";
            while ( getline( hypBiFile, line ) )
            { 
                hyp_bi_data = hyp_bi_data + line + "\n";
            }
        }
    }

    ifstream refFile(ref_file);

    if ( refFile ) 
    {
//         std::string ligne; 
        ref_data="";
        line="";
        while ( std::getline( refFile, line ) )
        { 
            ref_data = ref_data + line + "\n";
        }
    }
  
    double maxER=-1.0;
    bool human=false;
    MwerSegmenter ms(maxER, human);
    string result;
    ms.mwerAlign(ref_data, hyp_data, result);
    ofstream outFile(out_file);
    if ( outFile ) 
    {    
        outFile << result << endl;
    }   
//     std::cout << result << "\n";
}
