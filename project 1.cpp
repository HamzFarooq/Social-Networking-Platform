#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

struct Comment {
    string author;
    string content;
};

struct Post {
    string author;
    string content;
    vector<Comment> comments;
};

struct User {
    string username;
    string password;
    vector<string> friends;
    vector<string> friendRequests;
};

vector<User> users;
vector<Post> posts;
User* loggedInUser = nullptr;

// --- File Operations ---

void saveUsers() {
    ofstream fout("users.txt");
    for (auto &u : users) {
        fout << u.username << "\n" << u.password << "\n";

        // Save friends count and list
        fout << u.friends.size() << "\n";
        for (auto &f : u.friends) {
            fout << f << "\n";
        }

        // Save friendRequests count and list
        fout << u.friendRequests.size() << "\n";
        for (auto &r : u.friendRequests) {
            fout << r << "\n";
        }
    }
    fout.close();
}

void loadUsers() {
    ifstream fin("users.txt");
    if (!fin) return;

    users.clear();
    string uname, pwd;
    size_t fCount, rCount;
    while (getline(fin, uname)) {
        getline(fin, pwd);
        User u;
        u.username = uname;
        u.password = pwd;

        fin >> fCount;
        fin.ignore();
        for (size_t i = 0; i < fCount; i++) {
            string fr;
            getline(fin, fr);
            u.friends.push_back(fr);
        }

        fin >> rCount;
        fin.ignore();
        for (size_t i = 0; i < rCount; i++) {
            string req;
            getline(fin, req);
            u.friendRequests.push_back(req);
        }

        users.push_back(u);
    }
    fin.close();
}

void savePosts() {
    ofstream fout("posts.txt");
    for (auto &p : posts) {
        fout << p.author << "\n" << p.content << "\n";

        fout << p.comments.size() << "\n";
        for (auto &c : p.comments) {
            fout << c.author << "\n" << c.content << "\n";
        }
    }
    fout.close();
}

void loadPosts() {
    ifstream fin("posts.txt");
    if (!fin) return;

    posts.clear();
    string author, content, cauthor, ccontent;
    size_t commentsCount;
    while (getline(fin, author)) {
        getline(fin, content);
        fin >> commentsCount;
        fin.ignore();

        Post p;
        p.author = author;
        p.content = content;

        for (size_t i = 0; i < commentsCount; i++) {
            getline(fin, cauthor);
            getline(fin, ccontent);
            p.comments.push_back({cauthor, ccontent});
        }
        posts.push_back(p);
    }
    fin.close();
}

// --- Core Features ---

void signup() {
    string uname, pwd;
    cout << "Signup - Enter username: ";
    cin >> uname;
    cout << "Enter password: ";
    cin >> pwd;

    for (auto &u : users) {
        if (u.username == uname) {
            cout << "Username already taken!\n";
            return;
        }
    }

    users.push_back({uname, pwd});
    cout << "Signup successful! You can now login.\n";
}

bool login() {
    string uname, pwd;
    cout << "Login - Enter username: ";
    cin >> uname;
    cout << "Enter password: ";
    cin >> pwd;

    for (auto &u : users) {
        if (u.username == uname && u.password == pwd) {
            loggedInUser = &u;
            cout << "Welcome, " << loggedInUser->username << "!\n";
            return true;
        }
    }
    cout << "Invalid credentials!\n";
    return false;
}

void createPost() {
    cin.ignore();
    string content;
    cout << "Enter your post content: ";
    getline(cin, content);

    posts.push_back({loggedInUser->username, content});
    cout << "Post created successfully.\n";
}

void viewPosts() {
    if (posts.empty()) {
        cout << "No posts available.\n";
        return;
    }
    cout << "All Posts:\n";
    for (size_t i = 0; i < posts.size(); i++) {
        cout << i+1 << ". " << posts[i].author << ": " << posts[i].content << "\n";
        if (!posts[i].comments.empty()) {
            cout << "   Comments:\n";
            for (auto &c : posts[i].comments) {
                cout << "     - " << c.author << ": " << c.content << "\n";
            }
        }
    }
}

void addComment() {
    if (posts.empty()) {
        cout << "No posts to comment on.\n";
        return;
    }

    cout << "Enter post number to comment on (1 to " << posts.size() << "): ";
    int postNum; 
    cin >> postNum;
    if (postNum < 1 || postNum > (int)posts.size()) {
        cout << "Invalid post number!\n";
        return;
    }

    cin.ignore();
    string commentText;
    cout << "Enter your comment: ";
    getline(cin, commentText);

    posts[postNum - 1].comments.push_back({loggedInUser->username, commentText});
    cout << "Comment added.\n";
}

// Friend Requests and Friends

void sendFriendRequest() {
    string friendName;
    cout << "Enter username to send friend request: ";
    cin >> friendName;

    if (friendName == loggedInUser->username) {
        cout << "You can't send friend request to yourself!\n";
        return;
    }

    User* friendUser = nullptr;
    for (auto &u : users) {
        if (u.username == friendName) {
            friendUser = &u;
            break;
        }
    }
    if (!friendUser) {
        cout << "User not found!\n";
        return;
    }

    for (auto &f : loggedInUser->friends) {
        if (f == friendName) {
            cout << "You are already friends.\n";
            return;
        }
    }

    for (auto &req : friendUser->friendRequests) {
        if (req == loggedInUser->username) {
            cout << "Friend request already sent.\n";
            return;
        }
    }

    friendUser->friendRequests.push_back(loggedInUser->username);
    cout << "Friend request sent to " << friendName << ".\n";
}

void handleFriendRequests() {
    if (loggedInUser->friendRequests.empty()) {
        cout << "No friend requests.\n";
        return;
    }

    cout << "Friend Requests:\n";
    for (size_t i = 0; i < loggedInUser->friendRequests.size(); i++) {
        cout << i+1 << ". " << loggedInUser->friendRequests[i] << "\n";
    }
    cout << "Enter request number to accept (0 to cancel): ";
    int choice; cin >> choice;
    if (choice < 1 || choice > (int)loggedInUser->friendRequests.size()) {
        cout << "Cancelled or invalid choice.\n";
        return;
    }

    string requester = loggedInUser->friendRequests[choice - 1];

    loggedInUser->friends.push_back(requester);
    for (auto &u : users) {
        if (u.username == requester) {
            u.friends.push_back(loggedInUser->username);
            break;
        }
    }

    loggedInUser->friendRequests.erase(loggedInUser->friendRequests.begin() + (choice - 1));
    cout << "You are now friends with " << requester << "!\n";
}

void viewFriends() {
    if (loggedInUser->friends.empty()) {
        cout << "You have no friends yet.\n";
        return;
    }
    cout << "Your Friends:\n";
    for (auto &f : loggedInUser->friends) {
        cout << "- " << f << "\n";
    }
}

// Dashboard menu and main

void dashboard() {
    int choice;
    do {
        cout << "\nDashboard - Choose an option:\n";
        cout << "1. Create Post\n2. View Posts\n3. Add Comment\n4. Send Friend Request\n5. Friend Requests\n6. View Friends\n7. Logout\nYour choice: ";
        cin >> choice;

        switch(choice) {
            case 1: createPost(); break;
            case 2: viewPosts(); break;
            case 3: addComment(); break;
            case 4: sendFriendRequest(); break;
            case 5: handleFriendRequests(); break;
            case 6: viewFriends(); break;
            case 7:
                cout << "Logging out...\n";
                loggedInUser = nullptr;
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while(choice != 7);
}

int main() {
    loadUsers();
    loadPosts();

    int choice;
    do {
        cout << "\nWelcome to Social Network\n1. Signup\n2. Login\n3. Exit\nYour choice: ";
        cin >> choice;

        switch(choice) {
            case 1: signup(); break;
            case 2:
                if (login()) {
                    dashboard();
                    saveUsers();
                    savePosts();
                }
                break;
            case 3: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while(choice != 3);

    return 0;
}