/********************************************************
 * tree- Tree b in disk                                 *
 *                                                      *
 * Author:  Lucas Batista dos Santos                    *
 *                                                      *
 ********************************************************/
#include <iostream>
#include "tree.h"
#include "charSerial.h"
using namespace std;

int main()
{
    charSerial inserting[] = {')', '6', 'O', 'b', 'L', 'G', 'P', 'C', '3', '%', 'm', '(', 'H', 'W', 'Y', '&', 'k', '1', 'X', '-'};
    charSerial removing[] = {'C', 'm', '1', 'O', ')', 'W', 'Y', 'L', 'X', '('};
    tree<charSerial, 3> tree("Data","char", 1);
    node<charSerial, 3> n;
    int option;
    char k;
    bool quit = false;
    do {
        cout << "\nB-tree Testing\n" << endl;
        cout << "1. Insert a character key" << endl;
        cout << "2. Remove a character key" << endl;
        cout << "3. Search for a key" << endl;
        cout << "4. Print the tree" << endl;
        cout << "5. Run test-cases" << endl;
        cout << "6. Quit" << endl;
        cout << "\n> ";
        cin >> option;
        switch(option) {
        case 1:
            cout << "Enter an character value to insert: ";
            cin >> k;
            tree.insert(k);
            break;
        case 2:
            cout << "Enter an character value to insert: ";
            cin >> k;
            tree.remove(k);
            break;
        case 3:
            cout << "Enter an character value to search: ";
            cin >> k;
            if(tree.search(n, k) >= 0) {
                cout << "The char " << k << " was found into the tree." << endl;
            } else {
                cout << "The char " << k << " wasn't found into the tree." << endl;
            }
            break;
        case 4:
            tree.print();
            break;
        case 5:
            cout << "Insertion Test-Cases" << endl;
            for(charSerial i : inserting) {
                cout << "\nInserting character " << i.getCharacter() << ":" << endl;
                tree.insert(i);
                tree.print();
            }
            cout << "\nRemoval Test-Cases" << endl;
            for (charSerial i : removing) {
                cout << "\nRemoving charater " << i.getCharacter() << ":" << endl;
                tree.remove(i);
                tree.print();
            }
            break;
         case 6:
            quit = true;
            break;
         default:
            cout << "Invalid option. Please try again.\n" << endl;
            break;
        }
    } while(!quit);

    return 0;
}
