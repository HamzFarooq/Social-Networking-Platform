#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <algorithm>
using namespace std;

// --- Comment Class ---
class Comment {
    string author;
    string content;

public:
    Comment(const string& author, const string& content) : author(author), content(content) {}
    string getAuthor() const { return author; }
    string getContent() const { return content; }

    void display() const {
        cout << "     - " << author << ": " << content << "\n";
    }

    friend class Post;
};

// --- Post Class ---
class Post {
    string author;
    string content;
    vector<Comment> comments;

public:
    Post(const string& author, const string& content) : author(author), content(content) {}

    void addComment(const Comment& comment) {
        comments.push_back(comment);
    }

    void display(int index) const {
        cout << index + 1 << ". " << author << ": " << content << "\n";
        if (!comments.empty()) {
            cout << "   Comments:\n";
            for (const auto& c : comments) {
                c.display();
            }
        }
    }

    void save(ofstream& fout) const {
        fout << author << "\n" << content << "\n" << comments.size() << "\n";
        for (const auto& c : comments) {
            fout << c.getAuthor() << "\n" << c.getContent() << "\n";
        }
    }

    static Post load(ifstream& fin) {
        string author, content;
        getline(fin, author);
        getline(fin, content);
        size_t commentCount;
        fin >> commentCount;
        fin.ignore();

        Post p(author, content);
        for (size_t i = 0; i < commentCount; ++i) {
            string cauthor, ccontent;
            getline(fin, cauthor);
            getline(fin, ccontent);
            p.addComment(Comment(cauthor, ccontent));
        }
        return p;
    }
};

// --- User Class ---
class User {
    string username;
    string password;
    vector<string> friends;
    vector<string> friendRequests;

public:
    User() = default;
    User(string uname, string pwd) : username(uname), password(pwd) {}

    string getUsername() const { return username; }

    bool authenticate(const string& pwd) const {
        return password == pwd;
    }

    void addFriend(const string& uname) {
        friends.push_back(uname);
    }

    bool isFriendWith(const string& uname) const {
        return find(friends.begin(), friends.end(), uname) != friends.end();
    }

    void receiveRequest(const string& from) {
        if (find(friendRequests.begin(), friendRequests.end(), from) == friendRequests.end())
            friendRequests.push_back(from);
    }

    void showFriendRequests(vector<User>& users) {
        if (friendRequests.empty()) {
            cout << "No friend requests.\n";
            return;
        }

        cout << "Friend Requests:\n";
        for (size_t i = 0; i < friendRequests.size(); ++i) {
            cout << i + 1 << ". " << friendRequests[i] << "\n";
        }

        int choice;
        cout << "Enter request number to accept (0 to cancel): ";
        cin >> choice;
        if (choice < 1 || choice > (int)friendRequests.size()) {
            cout << "Cancelled or invalid choice.\n";
            return;
        }

        string requester = friendRequests[choice - 1];
        addFriend(requester);
        for (auto& u : users) {
            if (u.username == requester) {
                u.addFriend(username);
                break;
            }
        }

        friendRequests.erase(friendRequests.begin() + (choice - 1));
        cout << "You are now friends with " << requester << "!\n";
    }

    void showFriends() const {
        if (friends.empty()) {
            cout << "You have no friends yet.\n";
            return;
        }

        cout << "Your Friends:\n";
        for (const auto& f : friends) {
            cout << "- " << f << "\n";
        }
    }

    void save(ofstream& fout) const {
        fout << username << "\n" << password << "\n";
        fout << friends.size() << "\n";
        for (const auto& f : friends)
            fout << f << "\n";

        fout << friendRequests.size() << "\n";
        for (const auto& r : friendRequests)
            fout << r << "\n";
    }

    static User load(ifstream& fin) {
        string uname, pwd;
        size_t fCount, rCount;
        getline(fin, uname);
        getline(fin, pwd);

        User u(uname, pwd);

        fin >> fCount;
        fin.ignore();
        for (size_t i = 0; i < fCount; ++i) {
            string fr;
            getline(fin, fr);
            u.friends.push_back(fr);
        }

        fin >> rCount;
        fin.ignore();
        for (size_t i = 0; i < rCount; ++i) {
            string req;
            getline(fin, req);
            u.friendRequests.push_back(req);
        }

        return u;
    }

    friend class SocialNetwork;
};

// --- Polymorphic Menu Option ---
class MenuOption {
public:
    virtual void execute() = 0;
    virtual ~MenuOption() = default;
};

// --- SocialNetwork Class ---
class SocialNetwork {
    vector<User> users;
    vector<Post> posts;
    User* loggedInUser = nullptr;

public:
    void run() {
        loadUsers();
        loadPosts();

        int choice;
        do {
            cout << "\nWelcome to Social Network\n1. Signup\n2. Login\n3. Exit\nYour choice: ";
            cin >> choice;
            switch (choice) {
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
        } while (choice != 3);
    }

private:
    void signup() {
        string uname, pwd;
        cout << "Signup - Enter username: "; cin >> uname;
        cout << "Enter password: "; cin >> pwd;

        for (const auto& u : users) {
            if (u.getUsername() == uname) {
                cout << "Username already taken!\n";
                return;
            }
        }

        users.emplace_back(uname, pwd);
        cout << "Signup successful!\n";
    }

    bool login() {
        string uname, pwd;
        cout << "Login - Enter username: "; cin >> uname;
        cout << "Enter password: "; cin >> pwd;

        for (auto& u : users) {
            if (u.getUsername() == uname && u.authenticate(pwd)) {
                loggedInUser = &u;
                cout << "Welcome, " << uname << "!\n";
                return true;
            }
        }

        cout << "Invalid credentials!\n";
        return false;
    }

    void dashboard() {
        int choice;
        do {
            cout << "\nDashboard:\n";
            cout << "1. Create Post\n2. View Posts\n3. Add Comment\n4. Send Friend Request\n5. Handle Friend Requests\n6. View Friends\n7. Logout\nYour choice: ";
            cin >> choice;

            switch (choice) {
                case 1: createPost(); break;
                case 2: viewPosts(); break;
                case 3: addComment(); break;
                case 4: sendFriendRequest(); break;
                case 5: loggedInUser->showFriendRequests(users); break;
                case 6: loggedInUser->showFriends(); break;
                case 7: cout << "Logging out...\n"; loggedInUser = nullptr; break;
                default: cout << "Invalid choice!\n";
            }
        } while (choice != 7);
    }

    void createPost() {
        cin.ignore();
        string content;
        cout << "Enter your post content: ";
        getline(cin, content);
        posts.emplace_back(loggedInUser->getUsername(), content);
        cout << "Post created.\n";
    }

    void viewPosts() const {
        if (posts.empty()) {
            cout << "No posts available.\n";
            return;
        }

        for (size_t i = 0; i < posts.size(); ++i) {
            posts[i].display(i);
        }
    }

    void addComment() {
        if (posts.empty()) {
            cout << "No posts available.\n";
            return;
        }

        int postNum;
        cout << "Enter post number to comment on: ";
        cin >> postNum;

        if (postNum < 1 || postNum > (int)posts.size()) {
            cout << "Invalid post number!\n";
            return;
        }

        cin.ignore();
        string comment;
        cout << "Enter your comment: ";
        getline(cin, comment);

        posts[postNum - 1].addComment(Comment(loggedInUser->getUsername(), comment));
        cout << "Comment added.\n";
    }

    void sendFriendRequest() {
        string target;
        cout << "Enter username to send request: ";
        cin >> target;

        if (target == loggedInUser->getUsername()) {
            cout << "Cannot send request to yourself.\n";
            return;
        }

        for (auto& u : users) {
            if (u.getUsername() == target) {
                if (loggedInUser->isFriendWith(target)) {
                    cout << "Already friends.\n";
                    return;
                }
                u.receiveRequest(loggedInUser->getUsername());
                cout << "Request sent.\n";
                return;
            }
        }

        cout << "User not found.\n";
    }

    void saveUsers() {
        ofstream fout("users.txt");
        for (const auto& u : users) {
            u.save(fout);
        }
    }

    void loadUsers() {
        ifstream fin("users.txt");
        if (!fin) return;

        users.clear();
        while (fin.peek() != EOF) {
            users.push_back(User::load(fin));
        }
    }

    void savePosts() {
        ofstream fout("posts.txt");
        for (const auto& p : posts) {
            p.save(fout);
        }
    }

    void loadPosts() {
        ifstream fin("posts.txt");
        if (!fin) return;

        posts.clear();
        while (fin.peek() != EOF) {
            posts.push_back(Post::load(fin));
        }
    }
};

// --- Main ---
int main() {
    SocialNetwork app;
    app.run();
    return 0;
}