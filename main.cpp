
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <iomanip>
#include <cctype>

using namespace std;

struct Book {
    int id;
    string title;
    string author;
    bool available;
    int issuedTo;
};

struct Member {
    int id;
    string name;
};

class LibraryManagementSystem {
private:
    vector<Book> books;
    vector<Member> members;

    const string BOOK_FILE = "books.txt";
    const string MEMBER_FILE = "members.txt";

    void loadBooks() {
        books.clear();
        ifstream file(BOOK_FILE);
        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            string idText, title, author, availableText, issuedText;

            if (getline(ss, idText, '|') &&
                getline(ss, title, '|') &&
                getline(ss, author, '|') &&
                getline(ss, availableText, '|') &&
                getline(ss, issuedText)) {
                try {
                    books.push_back({
                        stoi(idText), title, author,
                        stoi(availableText) == 1, stoi(issuedText)
                    });
                } catch (...) {
                    cerr << "Skipping an invalid book record.\n";
                }
            }
        }
    }

    void saveBooks() const {
        ofstream file(BOOK_FILE);
        for (const Book& book : books) {
            file << book.id << '|'
                 << book.title << '|'
                 << book.author << '|'
                 << (book.available ? 1 : 0) << '|'
                 << book.issuedTo << '\n';
        }
    }

    void loadMembers() {
        members.clear();
        ifstream file(MEMBER_FILE);
        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            string idText, name;

            if (getline(ss, idText, '|') && getline(ss, name)) {
                try {
                    members.push_back({stoi(idText), name});
                } catch (...) {
                    cerr << "Skipping an invalid member record.\n";
                }
            }
        }
    }

    void saveMembers() const {
        ofstream file(MEMBER_FILE);
        for (const Member& member : members) {
            file << member.id << '|' << member.name << '\n';
        }
    }

    int readInt(const string& prompt) const {
        int value;

        while (true) {
            cout << prompt;

            if (cin >> value) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }

            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    string readText(const string& prompt) const {
        string value;

        while (true) {
            cout << prompt;
            getline(cin, value);

            if (value.empty()) {
                cout << "This field cannot be empty.\n";
                continue;
            }

            if (value.find('|') != string::npos) {
                cout << "The character | is not allowed.\n";
                continue;
            }

            return value;
        }
    }

    int findBookIndex(int id) const {
        for (int i = 0; i < static_cast<int>(books.size()); ++i) {
            if (books[i].id == id)
                return i;
        }
        return -1;
    }

    int findMemberIndex(int id) const {
        for (int i = 0; i < static_cast<int>(members.size()); ++i) {
            if (members[i].id == id)
                return i;
        }
        return -1;
    }

public:
    LibraryManagementSystem() {
        loadBooks();
        loadMembers();
    }

    void addBook() {
        int id = readInt("Enter book ID: ");

        if (id <= 0 || findBookIndex(id) != -1) {
            cout << "Invalid or duplicate book ID.\n";
            return;
        }

        string title = readText("Enter book title: ");
        string author = readText("Enter author name: ");

        books.push_back({id, title, author, true, 0});
        saveBooks();

        cout << "Book added successfully.\n";
    }

    void displayBooks() const {
        if (books.empty()) {
            cout << "No books found.\n";
            return;
        }

        cout << "\n--- BOOK CATALOGUE ---\n";
        cout << left << setw(8) << "ID"
             << setw(30) << "TITLE"
             << setw(25) << "AUTHOR"
             << "STATUS\n";

        cout << string(75, '-') << '\n';

        for (const Book& book : books) {
            cout << left << setw(8) << book.id
                 << setw(30) << book.title.substr(0, 28)
                 << setw(25) << book.author.substr(0, 23)
                 << (book.available ? "Available" : "Issued")
                 << '\n';
        }
    }

    void registerMember() {
        int id = readInt("Enter member ID: ");

        if (id <= 0 || findMemberIndex(id) != -1) {
            cout << "Invalid or duplicate member ID.\n";
            return;
        }

        string name = readText("Enter member name: ");

        members.push_back({id, name});
        saveMembers();

        cout << "Member registered successfully.\n";
    }

    void displayMembers() const {
        if (members.empty()) {
            cout << "No members registered.\n";
            return;
        }

        cout << "\n--- REGISTERED MEMBERS ---\n";

        for (const Member& member : members) {
            cout << "ID: " << member.id
                 << " | Name: " << member.name << '\n';
        }
    }

    void searchBook() const {
        string query = readText("Enter title or author to search: ");

        for (char& c : query) {
            c = static_cast<char>(
                tolower(static_cast<unsigned char>(c))
            );
        }

        bool found = false;

        for (const Book& book : books) {
            string title = book.title;
            string author = book.author;

            for (char& c : title)
                c = static_cast<char>(
                    tolower(static_cast<unsigned char>(c))
                );

            for (char& c : author)
                c = static_cast<char>(
                    tolower(static_cast<unsigned char>(c))
                );

            if (title.find(query) != string::npos ||
                author.find(query) != string::npos) {
                cout << "\nBook ID: " << book.id
                     << "\nTitle: " << book.title
                     << "\nAuthor: " << book.author
                     << "\nStatus: "
                     << (book.available ? "Available" : "Issued")
                     << "\n";

                found = true;
            }
        }

        if (!found)
            cout << "No matching books found.\n";
    }

    void issueBook() {
        int bookId = readInt("Enter book ID to issue: ");
        int bookIndex = findBookIndex(bookId);

        if (bookIndex == -1) {
            cout << "Book not found.\n";
            return;
        }

        if (!books[bookIndex].available) {
            cout << "This book is already issued.\n";
            return;
        }

        int memberId = readInt("Enter registered member ID: ");

        if (findMemberIndex(memberId) == -1) {
            cout << "Member not found. Register the member first.\n";
            return;
        }

        books[bookIndex].available = false;
        books[bookIndex].issuedTo = memberId;

        saveBooks();

        cout << "Book issued successfully to member ID "
             << memberId << ".\n";
    }

    void returnBook() {
        int bookId = readInt("Enter book ID to return: ");
        int bookIndex = findBookIndex(bookId);

        if (bookIndex == -1) {
            cout << "Book not found.\n";
            return;
        }

        if (books[bookIndex].available) {
            cout << "This book is not currently issued.\n";
            return;
        }

        int memberId = readInt("Enter member ID returning the book: ");

        if (books[bookIndex].issuedTo != memberId) {
            cout << "This book is not issued to that member.\n";
            return;
        }

        books[bookIndex].available = true;
        books[bookIndex].issuedTo = 0;

        saveBooks();

        cout << "Book returned successfully.\n";
    }

    void run() {
        while (true) {
            cout << "\n=================================\n";
            cout << "    LIBRARY MANAGEMENT SYSTEM\n";
            cout << "=================================\n";
            cout << "1. Add Book\n";
            cout << "2. Display All Books\n";
            cout << "3. Register Member\n";
            cout << "4. Display Members\n";
            cout << "5. Search by Title or Author\n";
            cout << "6. Issue Book\n";
            cout << "7. Return Book\n";
            cout << "0. Exit\n";

            int choice = readInt("Enter your choice: ");

            switch (choice) {
                case 1:
                    addBook();
                    break;
                case 2:
                    displayBooks();
                    break;
                case 3:
                    registerMember();
                    break;
                case 4:
                    displayMembers();
                    break;
                case 5:
                    searchBook();
                    break;
                case 6:
                    issueBook();
                    break;
                case 7:
                    returnBook();
                    break;
                case 0:
                    cout << "Thank you for using the Library Management System!\n";
                    return;
                default:
                    cout << "Invalid choice. Select a menu option.\n";
            }
        }
    }
};

int main() {
    LibraryManagementSystem library;
    library.run();
    return 0;
}
