//
// Created by Manju Muralidharan on 11/22/25.
//

//
// Created by Manju Muralidharan on 11/22/25.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include "tree.h"
using namespace std;

// Structure to hold raw story data from file
struct StoryNodeRaw{
    string id;
    string text;
    vector<string> children;
};

// Function to call OpenAI API using curl
string runCurlToOpenAI(const string &apiKey){
    // Create curl command
    string command =
        "curl https://api.openai.com/v1/responses "
        "-H \"Content-Type: application/json\" "
        "-H \"Authorization: Bearer " + apiKey + "\" "
        "-d '{"
        "\"model\": \"gpt-4.1-mini\","
        "\"input\": \"Generate a choose-your-own-adventure story. "
        "Format: [NODE_ID] TEXT: ... NEXT: child1, child2, ... "
        "Create 6-8 nodes. Start with node 1.\""
        "}' "
        " > story.txt 2> curl_error.txt";

    cout << "Fetching story from OpenAI..." << endl;
    int result = system(command.c_str());
    if (result != 0) {
        cout << "OpenAI API call failed. Using sample story." << endl;
        // Create sample story file
        ofstream fout("story.txt");
        fout << "[1] TEXT: You stand in a forest clearing. The path splits two ways. NEXT: 2, 3\n";
        fout << "[2] TEXT: You follow a narrow path through the trees. NEXT: 4\n";
        fout << "[3] TEXT: You enter a dark cave. It's cold and damp inside. NEXT: 4, 5\n";
        fout << "[4] TEXT: You find an abandoned hut. Smoke rises from the chimney. NEXT: 6\n";
        fout << "[5] TEXT: A river blocks your way. The water looks deep and fast. NEXT: 6\n";
        fout << "[6] TEXT: You reach the ancient ruins. Treasure glitters in the sunlight. NEXT:\n";
        fout.close();
    }

    return "story.txt";
}
// Function to read and parse the story file
vector<StoryNodeRaw> parseStoryFile(const string &filename){
    ifstream fin(filename);
    vector<StoryNodeRaw>nodes;

    if (!fin.is_open()){
        cout << "Error: Could not open story file." << endl;
        return nodes;
    }
    string line;
    while (getline(fin,line)){
        if (line.empty()) continue;
        StoryNodeRaw node;
        size_t bracketStart = line.find('[');
        size_t bracketEnd = line.find(']');
        if (bracketStart == string::npos || bracketEnd == string::npos) continue;
        node.id = line.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
        size_t textMarker = line.find("TEXT:");
        size_t nextMarker = line.find("NEXT:");
        if (textMarker == string::npos || nextMarker == string::npos) continue;
        string storyText = line.substr(textMarker + 5, nextMarker - (textMarker + 5));

        // Clean extra spaces
        while (!storyText.empty() && storyText[0] == ' '){
            storyText.erase(0, 1);
        }
        while (!storyText.empty() && storyText.back() == ' '){
            storyText.pop_back();
        }
        node.text = storyText;

        // Get child ID after NEXT:
        string childrenPart = line.substr(nextMarker + 5);
        stringstream ss(childrenPart);
        string childId;

        while (getline(ss,childId,',')){
            // Clean each child ID
            while (!childId.empty() && childId[0] == ' '){
                childId.erase(0, 1);
            }
            while (!childId.empty() && childId.back() == ' '){
                childId.pop_back();
            }

            if (!childId.empty()) {
                node.children.push_back(childId);
            }
        }

        nodes.push_back(node);
    }

    return nodes;
}

// Main function
int main() {
    cout << "=== Choose Your Own Adventure Game ===" << endl;
    cout << "Enter your OpenAI API key (press Enter for sample story): ";
    string apiKey;
    getline(cin,apiKey);

    string storyFile;
    if (apiKey.empty()){
        cout << "Using sample story..." << endl;
        // Create sample story file
        ofstream fout("story.txt");
        fout << "[1] TEXT: You stand in a forest clearing. The path splits two ways. NEXT: 2, 3\n";
        fout << "[2] TEXT: You follow a narrow path through the trees. NEXT: 4\n";
        fout << "[3] TEXT: You enter a dark cave. It's cold and damp inside. NEXT: 4, 5\n";
        fout << "[4] TEXT: You find an abandoned hut. Smoke rises from the chimney. NEXT: 6\n";
        fout << "[5] TEXT: A river blocks your way. The water looks deep and fast. NEXT: 6\n";
        fout << "[6] TEXT: You reach the ancient ruins. Treasure glitters in the sunlight. NEXT:\n";
        fout.close();
        storyFile = "story.txt";
    } else {
        storyFile = runCurlToOpenAI(apiKey);
    }

    // Parse the story file
    vector<StoryNodeRaw> storyData = parseStoryFile(storyFile);

    if (storyData.empty()) {
        cout << "Error: No valid story found." << endl;
        return 1;
    }

    // Create the adventure tree
    Tree<string> adventureTree;

    // Step 1: Create root from first node
    adventureTree.createRoot(storyData[0].id, storyData[0].text);

    // Step 2: Add all other nodes and connections
    for (const StoryNodeRaw& currentNode : storyData){
        // For each child of this node
        for (const string& childId : currentNode.children){
            // Find the child's text in the story data
            string childText = "";
            for (const StoryNodeRaw& childNode : storyData){
                if (childNode.id == childId) {
                    childText = childNode.text;
                    break;
                }
            }
            // Add the connection if we found the child
            if (!childText.empty()){
                adventureTree.addNode(currentNode.id,childId,childText);
            }
        }
    }

    cout << "\n✓ Story loaded into tree structure." << endl;
    // Show the tree structure
    adventureTree.printAll();
    // Start the game
    adventureTree.playGame();

    return 0;
}