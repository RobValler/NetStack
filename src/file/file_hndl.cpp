/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "file_hndl.h"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>

bool FileStreamRead(const std::string& filename, std::vector<char>& data) {

    bool result = false;
    std::ifstream file;
    file.open(filename, std::ifstream::binary);
    if(!file.fail()) {
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        // send data in one lump of data
        ///\ ToDo: add multi-phase messages
        data.resize(fileSize);
        file.read(&data[0], fileSize);
        result = true;
    }
    file.close();

    return result;
}

bool FileStreamWrite(const std::string& filename, const std::vector<char>& data) {

    bool result = false;
    std::ofstream file;
    std::filesystem::path localPath = filename;
    std::string fileName = localPath.filename().string();
    file.open(fileName, std::ifstream::binary);
    if(!file.fail()) {

        file.write(&data[0], data.size());
        file.close();
    }
    return result;
}

int XMLReadconst(std::string xml_filename, SXmlData& outgoing_data) {

    // Initialize libxml
    xmlInitParser();

    // Parse XML file
    xmlDocPtr file = xmlReadFile(xml_filename.c_str(), nullptr, 0);
    if (file == nullptr) {
        std::cerr << "Failed to parse XML file\n";
        return -1;
    }

    // Get root element
    xmlNodePtr root = xmlDocGetRootElement(file);

    // Loop through children of root
    for (xmlNodePtr node = root->children; node; node = node->next) {
        if (node->type == XML_ELEMENT_NODE &&
            xmlStrEqual(node->name, BAD_CAST "testcase")) {

            STestCase data;

            // Get attributes
            xmlChar* id = xmlGetProp(node, BAD_CAST "id");            
            xmlChar* name = xmlGetProp(node, BAD_CAST "name");
            // data.id =
            // data.name = name;

            std::cout << "Testcase ID: " << id << ", Name: " << name << "\n";

            // Loop through testcase children
            for (xmlNodePtr child = node->children; child; child = child->next) {
                if (child->type == XML_ELEMENT_NODE &&
                    xmlStrEqual(child->name, BAD_CAST "result")) {

                    xmlChar* result = xmlNodeGetContent(child);
                    std::cout << "  Result: " << result << "\n";
                    xmlFree(result);
                }
            }

            xmlFree(id);
            xmlFree(name);
        }
    }

    // Cleanup
    xmlFreeDoc(file);
    xmlCleanupParser();

    return 0;
}

