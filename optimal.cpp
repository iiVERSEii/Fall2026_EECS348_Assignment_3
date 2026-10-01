/*
============================================================================
Name            : EECS 348 Assignment 3
Author          : Kaden Shepherd
KUID            : 3177228
Description     : C++ program that prioritizes CEO emails using a custom MaxHeap.
Inputs          : Standard input file containing EMAIL, NEXT, READ, and COUNT commands.
Output          : Displays the next email, unread count, and empty-queue notice.
Collaborators   : None
Other Sources   : GPT.cpp used as a starting point; revised with AI assistance from VSCode integrated CoPilot.
Creation Date   : October 1, 2026
Revisions       : Added input parsing checks, whitespace trimming, and comments.
============================================================================
*/



#include <iostream> // Provides standard input and output streams.
#include <string>   // Provides the string type used for email data and commands.
#include <sstream>  // Provides string-stream utilities.
#include <vector>   // Provides list-like storage for the custom heap.

using namespace std; // Allows standard-library names without the std:: prefix.

// Removes whitespace from both ends of a string.
string trim(const string& value)
{
    const string whitespace = " \t\r\n"; // Whitespace characters to remove.
    size_t first = value.find_first_not_of(whitespace); // Locate the first non-whitespace character.
    if (first == string::npos)
    {
        return ""; // Return empty when the input contains only whitespace.
    }

    size_t last = value.find_last_not_of(whitespace); // Locate the last non-whitespace character.
    return value.substr(first, last - first + 1); // Return the trimmed substring.
}

// ------------------------------------------------------------
// Email class
// Represents one email in the CEO's inbox.
// ------------------------------------------------------------
class Email
{
private:
    string sender; // Stores the sender category.
    string subject; // Stores the email subject line.
    string date; // Stores the date in MM-DD-YYYY format.

public:
    Email()
    {
        sender = ""; // Initialize the sender as empty.
        subject = ""; // Initialize the subject as empty.
        date = ""; // Initialize the date as empty.
    }

    Email(string s, string sub, string d) // Construct an email from its three fields.
    {
        sender = s; // Save the sender category.
        subject = sub; // Save the subject line.
        date = d; // Save the date string.
    }

    string getSender() // Return the sender category.
    {
        return sender; // Provide the stored sender.
    }

    string getSubject() // Return the email subject.
    {
        return subject; // Provide the stored subject.
    }

    string getDate() // Return the email date.
    {
        return date; // Provide the stored date.
    }

    // Returns the priority of the sender category.
    // Higher number = higher priority.
    int getSenderPriority() // Map sender categories to heap priority values.
    {
        if (sender == "Boss")
            return 5; // Boss emails have the highest priority.
        else if (sender == "Subordinate")
            return 4; // Subordinate emails have the next priority.
        else if (sender == "Peer")
            return 3; // Peer emails have the middle priority.
        else if (sender == "ImportantPerson")
            return 2; // ImportantPerson emails have the next-lowest priority.
        else
            return 1; // OtherPerson emails have the lowest priority.
    }

    // Converts MM-DD-YYYY into YYYYMMDD.
    // This makes dates easy to compare numerically.
    int getDateValue() // Convert MM-DD-YYYY into YYYYMMDD for comparison.
    {
        int month = stoi(date.substr(0, 2)); // Read the two-digit month.
        int day = stoi(date.substr(3, 2)); // Read the two-digit day.
        int year = stoi(date.substr(6, 4)); // Read the four-digit year.

        return year * 10000 + month * 100 + day; // Make later dates compare as larger values.
    }

    // Determines whether this email has a higher priority
    // than another email.
    bool higherPriorityThan(Email other) // Compare this email with another email.
    {
        // First compare sender category.
        if (getSenderPriority() != other.getSenderPriority())
        {
            return getSenderPriority() > other.getSenderPriority(); // Higher sender rank wins.
        }

        // If sender category is the same, newer date wins.
        if (getDateValue() != other.getDateValue())
        {
            return getDateValue() > other.getDateValue(); // For a category tie, the newer date wins.
        }

        // If everything is equal, neither has priority.
        return false; // Equal category and date means neither email outranks the other.
    }
};


// ------------------------------------------------------------
// MaxHeap class
// List-based implementation of a MaxHeap.
// The vector is used as the list that stores the heap.
// ------------------------------------------------------------
class MaxHeap
{
private:
    vector<Email> heap; // Stores heap elements in array-style list order.

public:

    // Returns the index of the parent.
    int parent(int index) // Find the parent index in the binary heap.
    {
        return (index - 1) / 2; // Parent index formula for a zero-based heap.
    }

    // Returns the index of the left child.
    int leftChild(int index) // Find the left-child index.
    {
        return (2 * index) + 1; // Left-child index formula for a zero-based heap.
    }

    // Returns the index of the right child.
    int rightChild(int index) // Find the right-child index.
    {
        return (2 * index) + 2; // Right-child index formula for a zero-based heap.
    }

    // Moves an item upward until the MaxHeap property is restored.
    void heapifyUp(int index) // Restore heap order after inserting an item.
    {
        while (index > 0) // Continue while the current item has a parent.
        {
            int parentIndex = parent(index); // Find the current item's parent.

            if (heap[index].higherPriorityThan(heap[parentIndex]))
            {
                Email temporary = heap[index]; // Preserve the child during the swap.
                heap[index] = heap[parentIndex]; // Move the higher-priority parent down.
                heap[parentIndex] = temporary; // Move the child up to its parent's position.

                index = parentIndex; // Continue from the item's new position.
            }
            else
            {
                break; // Stop when the parent already has higher priority.
            }
        }
    }

    // Moves an item downward until the MaxHeap property is restored.
    void heapifyDown(int index) // Restore heap order after removing the root.
    {
        int size = heap.size(); // Cache the number of items for child-bound checks.

        while (true) // Continue until the current node outranks its children.
        {
            int left = leftChild(index); // Find the left child's index.
            int right = rightChild(index); // Find the right child's index.
            int largest = index; // Initially treat the current node as the largest.

            if (left < size &&
                heap[left].higherPriorityThan(heap[largest]))
            {
                largest = left; // Select the left child when it has higher priority.
            }

            if (right < size &&
                heap[right].higherPriorityThan(heap[largest]))
            {
                largest = right; // Select the right child when it outranks the current largest.
            }

            if (largest != index)
            {
                Email temporary = heap[index]; // Preserve the current item during the swap.
                heap[index] = heap[largest]; // Move the highest-priority child upward.
                heap[largest] = temporary; // Move the current item downward.

                index = largest; // Continue from the item's new position.
            }
            else
            {
                break; // Stop when neither child outranks the current item.
            }
        }
    }

    // Adds an email to the MaxHeap.
    void insert(Email email) // Add an email and restore the MaxHeap property.
    {
        heap.push_back(email); // Append the new email to the end of the vector.

        int index = heap.size() - 1; // Find the new email's index.
        heapifyUp(index); // Move it upward as needed.
    }

    // Returns the highest-priority email without deleting it.
    Email peek() // Return the highest-priority email without removing it.
    {
        return heap[0]; // The root of a MaxHeap has the highest priority.
    }

    // Removes the highest-priority email.
    Email removeMax() // Remove and return the highest-priority email.
    {
        Email highestPriority = heap[0]; // Save the root to return it after removal.

        heap[0] = heap[heap.size() - 1]; // Move the last item into the root position.
        heap.pop_back(); // Remove its old final position.

        if (heap.size() > 0)
        {
            heapifyDown(0); // Restore heap order from the root downward.
        }

        return highestPriority; // Return the item removed from the heap.
    }

    // Returns the number of unread emails.
    int size() // Return the number of emails in the heap.
    {
        return heap.size(); // The vector length is the heap size.
    }

    // Returns true if there are no emails.
    bool isEmpty() // Check whether the heap contains no emails.
    {
        return heap.empty(); // Use the vector's empty check.
    }
};


// ------------------------------------------------------------
// CEOInbox class
// Handles the CEO's commands and uses the MaxHeap.
// ------------------------------------------------------------
class CEOInbox
{
private:
    MaxHeap emailQueue; // Stores inbox emails in priority order.

public:

    // Adds an email to the CEO's inbox.
    void addEmail(string sender, string subject, string date) // Create and enqueue one email.
    {
        Email newEmail(sender, subject, date); // Package the fields into an Email object.
        emailQueue.insert(newEmail); // Add the email to the MaxHeap.
    }

    // Displays the highest-priority email without removing it.
    void nextEmail() // Display the top email without removing it.
    {
        if (emailQueue.isEmpty()) // Check before accessing the heap root.
        {
            cout << "No emails in queue." << endl; // Report the empty inbox.
            return; // Stop before trying to peek.
        }

        Email email = emailQueue.peek(); // Retrieve the highest-priority email.

        cout << "Next email:" << endl; // Print the heading for the email.
        cout << "Sender: " << email.getSender() << endl; // Print the sender category.
        cout << "Subject: " << email.getSubject() << endl; // Print the subject line.
        cout << "Date: " << email.getDate() << endl; // Print the date.
    }

    // Removes the highest-priority email.
    void readEmail() // Remove the highest-priority email from the inbox.
    {
        if (emailQueue.isEmpty()) // Check before attempting removal.
        {
            return; // Nothing needs to be removed from an empty inbox.
        }

        emailQueue.removeMax(); // Remove the heap root, even if NEXT was not called.
    }

    // Displays the number of unread emails.
    void countEmails() // Display the number of unread emails.
    {
        cout << "There are " << emailQueue.size()
             << " emails to read." << endl; // Print the current queue size.
    }

    // Processes one command from the input.
    void processCommand(string command) // Dispatch a non-EMAIL command.
    {
        if (command == "NEXT") // NEXT displays the queue's current maximum.
        {
            nextEmail(); // Show, but do not remove, the top email.
        }
        else if (command == "READ") // READ removes the queue's current maximum.
        {
            readEmail(); // Remove the highest-priority email.
        }
        else if (command == "COUNT") // COUNT displays the unread-email total.
        {
            countEmails(); // Print the queue size.
        }
    }
};


// ------------------------------------------------------------
// Main program
// ------------------------------------------------------------
int main() // Read the test input and apply each command to the inbox.
{
    CEOInbox inbox; // Create the CEO's inbox and its priority queue.

    string line; // Holds one line read from standard input.

    // Read the entire test file until EOF.
    while (getline(cin, line)) // Continue processing until the input reaches EOF.
    {
        line = trim(line); // Remove surrounding whitespace before parsing.

        // Ignore empty lines.
        if (line.empty()) // Skip blank lines.
        {
            continue; // Read the next line instead.
        }

        // ----------------------------------------------------
        // EMAIL command
        // ----------------------------------------------------
        if (line.substr(0, 6) == "EMAIL ") // Parse an email insertion command.
        {
            // Remove "EMAIL " from the beginning.
            string emailData = line.substr(6); // Remove the EMAIL command prefix.

            // Find the first comma.
            size_t firstComma = emailData.find(','); // Find the separator after the sender.
            if (firstComma == string::npos) // Reject a line missing its first separator.
            {
                continue; // Ignore this malformed email line.
            }

            // Find the second comma.
            size_t secondComma = emailData.find(',', firstComma + 1); // Find the separator after the subject.
            if (secondComma == string::npos ||
                emailData.find(',', secondComma + 1) != string::npos) // Require exactly two separators.
            {
                continue; // Ignore missing or extra comma-delimited fields.
            }

            // Extract sender.
            string sender = trim(emailData.substr(0, firstComma)); // Extract and trim the sender.

            // Extract subject.
            string subject = trim(emailData.substr( // Extract and trim the subject field.
                firstComma + 1,
                secondComma - firstComma - 1
            ));

            // Extract date.
            string date = trim(emailData.substr(secondComma + 1)); // Extract and trim the date.

            if (sender.empty() || subject.empty() || date.empty()) // Reject any missing required field.
            {
                continue; // Do not enqueue an incomplete email.
            }

            // Add email to the MaxHeap.
            inbox.addEmail(sender, subject, date); // Insert the parsed email into the heap.
        }

        // ----------------------------------------------------
        // Other commands
        // ----------------------------------------------------
        else
        {
            inbox.processCommand(line); // Process NEXT, READ, or COUNT.
        }
    }

    return 0; // Signal successful completion.
}