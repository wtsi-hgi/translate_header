#include <htslib/sam.h>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cstdio>

class TranslateHeader {

public:
    void parse_args(int argc, char** argv) {
        if (argc < 5) {
            std::cout << "Arguments should be: translate_header <input.bam> <newheader.sam> <translation_table.txt> <output.bam>" << std::endl;
            throw new std::runtime_error("owwie");
        }
        
        file_name = argv[1];
        replace_name = argv[2];
        trans_name = argv[3];
        out_name = argv[4];
    }
    bool init() {
        // Open files
        file_iter = sam_open(file_name.c_str(), "rb", 0);
        replace_iter = sam_open(replace_name.c_str(), "r", 0);
        out_file = sam_open(out_name.c_str(), "wb", 0);
        
        
        if (file_iter == NULL || replace_iter == NULL || out_file == NULL) {
            return false;
        }
        
        // Iterate through BAM
        file_header = sam_hdr_read(file_iter);
        replace_header = sam_hdr_read(replace_iter);
        
        build_translation();

        return true;
    }

    void parse() {
        if (sam_hdr_write(out_file, replace_header) != 0) {
            throw new std::runtime_error("IEEEE!");
        }
        
        bam1_t* file_read = bam_init1();
        
        while (sam_read1(file_iter, file_header, file_read) >= 0) {
            file_read->core.tid = trans[file_read->core.tid];
            sam_write1(out_file, file_header, file_read);
        }

        // Clean up
        if (file_read) { bam_destroy1(file_read); }
    }
    
    ~TranslateHeader() {
        sam_close(out_file);
        sam_close(replace_iter);
        sam_close(file_iter);
        delete trans;
    }
    
private:
    void build_translation() {
        FILE* trans_file = fopen(trans_name.c_str(), "r");

        int file_entries = file_header->n_targets;
        int replace_entries = replace_header->n_targets;

        trans = new int[file_entries];
        
        char* linepointer = NULL;
        size_t read = 0;
        
        int counter = file_entries;

        while (!feof(trans_file) && !ferror(trans_file) && counter > 0) {
            getline(&linepointer, &read, trans_file);
            
            char* sep = linepointer;
            strsep(&sep, "\t");
            if (sep == NULL) break;
            char* two = sep;
            strsep(&two, "\t\n");

            
            // lookup tid of original and replacement
            int i = 0;
            for ( ; i < file_entries; i++ ) {
                char* item = file_header->target_name[i];
                if (!strcmp(item,linepointer)) { break; }
            }
            int j = 0;
            for ( ; j < replace_entries; j++ ) {
                char* item = replace_header->target_name[j];
                if (!strcmp(item,sep)) { break; }
            }
            
            trans[i] = j;
            counter--;
        }
        free(linepointer);
        
        fclose(trans_file);
    }

    std::string file_name;
    std::string replace_name;
    std::string trans_name;
    std::string out_name;

    samFile* file_iter;
    samFile* replace_iter;
    samFile* out_file;
    
    bam_hdr_t* file_header;
    bam_hdr_t* replace_header;
    
    int* trans;
};

int main(int argc, char** argv) {

    TranslateHeader* parser = new TranslateHeader();
    
    parser->parse_args(argc, argv);
    if (!parser->init()) return -1;
    
    parser->parse();
    
    delete parser;
      
    return 0;
}
