#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#define MAX_USERS 1000
#define MAX_TRANSACTIONS 1000
#define ADMIN_ID "admin"
#define ADMIN_PASSWORD "123"
#define MAX_RECENT 5

typedef struct {
    char name[50];
    char dob[11];
    char phone[11];
    char nationalID[20];
    char address[100];
    char password[20];
    char pin[5];
    float balance;
    int accountNumber;
} User;

typedef struct {
    int accountNumber;
    char type[20];
    float amount;
    char date[11];
    char time[9];
    int targetAccount;
} Transaction;

typedef struct {
    int accountNumber;
    char message[200];
    char reply[200];
    bool replied;
} Message;

User users[MAX_USERS];
Transaction transactions[MAX_TRANSACTIONS];
Message messages[MAX_USERS];
int userCount = 0;
int transactionCount = 0;
int messageCount = 0;

// Function Prototypes
void mainMenu();
void userMenu();
void registerUser();
void loginUser();
void adminLogin();
void userDashboard(int userIndex);
void depositMoney(int userIndex);
void withdrawMoney(int userIndex);
void transferMoney(int userIndex);
void viewAccountDetails(int userIndex);
void resetUserPassword(int userIndex);
void contactAdmin(int userIndex);
bool validateName(const char *name);
bool validateDOB(const char *dob);
bool validatePhone(const char *phone);
bool validateNationalID(const char *nationalID);
bool validatePassword(const char *password);
bool validatePIN(const char *pin);
bool isPhoneUnique(const char *phone);
bool isNationalIDUnique(const char *nationalID);
void getCurrentDateTime(char *date, char *timeStr);
void saveUsersToFile();
void loadUsersFromFile();
void saveTransactionsToFile();
void loadTransactionsFromFile();
void saveMessagesToFile();
void loadMessagesFromFile();
void saveUsersToCSV();
void loadUsersFromCSV();
void initTransactionCSV();
void appendTransactionToFile(Transaction transaction);
void appendMessageToFile(Message message);
void updateUserTxtFile(User user);
void ensureDataFolder();
void ensureUsersTxtFolder();
void editRegistrationFields(User *user);
void adminDashboard();
void viewAllAccounts();
void deleteUserAccount();
void resetUserPasswordAdmin();
void viewAndReplyMessages();
void viewTransactionLogs();
void broadcastMessage();
void editUserAccountInfo();
void generateUserStatement();
void viewBroadcastMessages();
void generateMyStatement(int userIndex);
void displayTransactions(int accountNumber, int month, int year);
void logAdminAction(char *action);
bool isAdminLoggedIn();

// Utility Functions
void ensureDataFolder() {
#ifdef _WIN32
    _mkdir("data");
#else
    mkdir("data", 0777);
#endif
}
void ensureUsersTxtFolder() {
#ifdef _WIN32
    _mkdir("users_txt");
#else
    mkdir("users_txt", 0777);
#endif
}
bool validateName(const char *name) {
  for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha(name[i]) && name[i] != ' ')
            return false;
    }
    return true;
}
bool validateDOB(const char *dob) {
    if (strlen(dob) != 10)
        return false;
    if (dob[2] != '-' || dob[5] != '-')
        return false;
  for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5)
            continue;
        if (!isdigit(dob[i]))
            return false;
    }
    return true;
}
bool validatePhone(const char *phone) {
    if (strlen(phone) != 10)
        return false;
  for (int i = 0; i < 10; i++) {
        if (!isdigit(phone[i]))
            return false;
    }
    return true;
}
bool isPhoneUnique(const char *phone) {
  for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].phone, phone) == 0)
            return false;
    }
    return true;
}
bool validateNationalID(const char *nationalID) {
  for (int i = 0; nationalID[i] != '\0'; i++) {
        if (!isdigit(nationalID[i]))
            return false;
    }
    return true;
}
bool isNationalIDUnique(const char *nationalID) {
  for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].nationalID, nationalID) == 0)
            return false;
    }
    return true;
}
bool validatePassword(const char *password) { return strlen(password) >= 6; }
bool validatePIN(const char *pin) {
    if (strlen(pin) != 4)
        return false;
  for (int i = 0; i < 4; i++) {
        if (!isdigit(pin[i]))
            return false;
    }
    return true;
}
void getCurrentDateTime(char *date, char *timeStr) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date, "%02d-%02d-%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    sprintf(timeStr, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// File I/O Functions
void saveUsersToFile() {
    FILE *file = fopen("data/users.dat", "wb");
    if (file == NULL)
        return;
    fwrite(&userCount, sizeof(int), 1, file);
    fwrite(users, sizeof(User), userCount, file);
    fclose(file);
}
void loadUsersFromFile() {
    FILE *file = fopen("data/users.dat", "rb");
    if (file == NULL)
        return;
    fread(&userCount, sizeof(int), 1, file);
    fread(users, sizeof(User), userCount, file);
    fclose(file);
}
void saveTransactionsToFile() {
    FILE *file = fopen("data/transactions.dat", "wb");
    if (file == NULL)
        return;
    fwrite(&transactionCount, sizeof(int), 1, file);
    fwrite(transactions, sizeof(Transaction), transactionCount, file);
    fclose(file);
}
void loadTransactionsFromFile() {
    FILE *file = fopen("data/transactions.dat", "rb");
    if (file == NULL)
        return;
    fread(&transactionCount, sizeof(int), 1, file);
    fread(transactions, sizeof(Transaction), transactionCount, file);
    fclose(file);
}
void saveMessagesToFile() {
    FILE *file = fopen("data/messages.dat", "wb");
    if (file == NULL)
        return;
    fwrite(&messageCount, sizeof(int), 1, file);
    fwrite(messages, sizeof(Message), messageCount, file);
    fclose(file);
}
void loadMessagesFromFile() {
    FILE *file = fopen("data/messages.dat", "rb");
    if (file == NULL)
        return;
    fread(&messageCount, sizeof(int), 1, file);
    fread(messages, sizeof(Message), messageCount, file);
    fclose(file);
}
void saveUsersToCSV() {
    FILE *csvFile = fopen("users.csv", "w");
    if (csvFile == NULL)
        return;
  fprintf(
      csvFile,
      "Name,DOB,Phone,NationalID,Address,AccountNumber,Balance,Password,PIN\n");
  for (int i = 0; i < userCount; i++) {
    fprintf(csvFile, "%s,%s,%s,%s,%s,%d,%.2f,%s,%s\n", users[i].name,
            users[i].dob, users[i].phone, users[i].nationalID, users[i].address,
            users[i].accountNumber, users[i].balance, users[i].password,
            users[i].pin);
    }
    fclose(csvFile);
}
void loadUsersFromCSV() {
    FILE *csvFile = fopen("users.csv", "r");
  if (csvFile == NULL) {
        userCount = 0;
        return;
    }
    char line[512];
    fgets(line, sizeof(line), csvFile); // Skip header
    userCount = 0;
  while (fgets(line, sizeof(line), csvFile)) {
        User u;
        sscanf(line, "%49[^,],%10[^,],%10[^,],%19[^,],%99[^,],%d,%f,%19[^,],%4s",
           u.name, u.dob, u.phone, u.nationalID, u.address, &u.accountNumber,
           &u.balance, u.password, u.pin);
        users[userCount++] = u;
    }
    fclose(csvFile);
}
void initTransactionCSV() {
    FILE *csvFile = fopen("transactions.csv", "r");
  if (csvFile == NULL) {
        csvFile = fopen("transactions.csv", "w");
    if (csvFile != NULL) {
            fprintf(csvFile, "AccountNumber,Date,Time,Type,Amount,TargetAccount\n");
            fclose(csvFile);
        }
  } else {
        fclose(csvFile);
    }
}
void appendTransactionToFile(Transaction transaction) {
    FILE *csvFile = fopen("transactions.csv", "a");
    if (csvFile == NULL)
        return;
  if (transaction.targetAccount == -1) {
    fprintf(csvFile, "%d,%s,%s,%s,%.2f,\n", transaction.accountNumber,
            transaction.date, transaction.time, transaction.type,
            transaction.amount);
  } else {
    fprintf(csvFile, "%d,%s,%s,%s,%.2f,%d\n", transaction.accountNumber,
            transaction.date, transaction.time, transaction.type,
            transaction.amount, transaction.targetAccount);
    }
    fclose(csvFile);
}
void appendMessageToFile(Message message) {
    FILE *file = fopen("messages.txt", "a");
    if (file == NULL)
        return;
  fprintf(file, "%d,%s,%s,%d\n", message.accountNumber, message.message,
            message.reply, message.replied);
    fclose(file);
}
void updateUserTxtFile(User user) {
    char txtFilename[64];
    sprintf(txtFilename, "users_txt/%d.txt", user.accountNumber);
    FILE *txtFile = fopen(txtFilename, "w");
    if (txtFile == NULL)
        return;
    fprintf(txtFile, "Name: %s\n", user.name);
    fprintf(txtFile, "DOB: %s\n", user.dob);
    fprintf(txtFile, "Phone: %s\n", user.phone);
    fprintf(txtFile, "National ID: %s\n", user.nationalID);
    fprintf(txtFile, "Address: %s\n", user.address);
    fprintf(txtFile, "Account Number: %d\n", user.accountNumber);
    fprintf(txtFile, "Balance: %.2f\n", user.balance);
    fprintf(txtFile, "Password: %s\n", user.password);
    fprintf(txtFile, "PIN: %s\n", user.pin);
    fclose(txtFile);
}

// Core Features (Menus and Main)
void userMenu() {
    int choice;
  while (1) {
        system("cls");
        printf("\n\n============================================");
        printf("\nWelcome Menu");
        printf("\n============================================");
        printf("\n1. Register");
        printf("\n2. Login");
        printf("\n3. Exit");
        printf("\n============================================");
        printf("\nEnter your choice: ");
    if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input, please enter a number.");
            while (getchar() != '\n')
                ;
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
            continue;
        }
    switch (choice) {
        case 1:
            registerUser();
            break;
        case 2:
            loginUser();
            break;
        case 3:
            exit(0);
        default:
            printf("\nInvalid option, try again");
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
        }
    }
}
void mainMenu() {
    int choice;
  while (1) {
        system("cls");
        printf("\n\n============================================");
        printf("\nWelcome to your bank");
        printf("\n============================================");
        printf("\n1. User");
        printf("\n2. Bank");
        printf("\n3. Exit");
        printf("\n============================================");
        printf("\nEnter your choice: ");
    if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input, please enter a number.");
            while (getchar() != '\n')
                ;
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
            continue;
        }
    switch (choice) {
        case 1:
            userMenu();
            break;
        case 2:
            adminLogin();
            break;
        case 3:
            saveUsersToFile();
            saveTransactionsToFile();
            saveMessagesToFile();
            exit(0);
        default:
            printf("\nInvalid option, try again");
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
        }
    }
}

void editRegistrationFields(User *user) {
    int editChoice;

    system(" ");
    printf("\n\n============================================");
    printf("\nEdit Registration");
    printf("\n============================================");
    printf("\n1. Name");
    printf("\n2. DOB");
    printf("\n3. Password");
    printf("\n4. Phone");
    printf("\n5. National ID");
    printf("\n6. Address");
    printf("\n7. PIN");
    printf("\n8. Cancel editing");
    printf("\n============================================");
    printf("\nEnter your choice: ");
    scanf("%d", &editChoice);

  switch (editChoice) {
    case 1: // Edit Name
    do {
            printf("\nEnter new name (max 19 characters): ");
            scanf(" %[^\n]", user->name);
      if (!validateName(user->name)) {
                printf("\nInvalid name. Please use alphabets only.");
            }
        } while (!validateName(user->name));
        break;

    case 2: // Edit DOB
    do {
            printf("\nEnter new date of birth (DD-MM-YYYY): ");
            scanf("%s", user->dob);
      if (!validateDOB(user->dob)) {
                printf("\nInvalid date format. Please use DD-MM-YYYY format.");
            }
        } while (!validateDOB(user->dob));
        break;

    case 3: // Edit Password
    do {
            printf("\nCreate a new password (min 6 chars): ");
            scanf("%s", user->password);
      if (!validatePassword(user->password)) {
                printf("\nPassword must be at least 6 characters long.");
            }
        } while (!validatePassword(user->password));
        break;

    case 4: // Edit Phone
    do {
            printf("\nEnter new phone number: ");
            scanf("%s", user->phone);
      if (!validatePhone(user->phone)) {
                printf("\nPhone number must be 10 digits.");
      } else if (!isPhoneUnique(user->phone)) {
                printf("\nPhone number already registered.");
            }
        } while (!validatePhone(user->phone) || !isPhoneUnique(user->phone));
        break;

    case 5: // Edit National ID
    do {
            printf("\nEnter new National ID: ");
            scanf("%s", user->nationalID);
      if (!validateNationalID(user->nationalID)) {
                printf("\nNational ID must be numeric.");
      } else if (!isNationalIDUnique(user->nationalID)) {
                printf("\nNational ID already registered.");
            }
    } while (!validateNationalID(user->nationalID) ||
             !isNationalIDUnique(user->nationalID));
        break;

    case 6: // Edit Address
        printf("\nEnter new Address: ");
        scanf(" %[^\n]", user->address);
        break;

    case 7: // Edit PIN
    do {
            printf("\nSet a new 4-digit PIN: ");
            scanf("%s", user->pin);
      if (!validatePIN(user->pin)) {
                printf("\nPIN must be 4 digits.");
            }
        } while (!validatePIN(user->pin));
        break;

    case 8: // Cancel editing
        printf("\nEditing cancelled.");
        printf("\nPress any key to continue...");
        getchar();
        getchar();
        break;

    default:
        printf("\nInvalid choice.");
        printf("\nPress any key to continue...");
        getchar();
        getchar();
    }
}

void registerUser() {
    User newUser;
    char confirm;

    system("cls");
    printf("\n\n============================================");
    printf("\nUser Registration");
    printf("\n============================================");

    // Name input and validation
  do {
        printf("\nEnter full name (max 19 characters): ");
        scanf(" %[^\n]", newUser.name);
    if (!validateName(newUser.name)) {
            printf("\nInvalid name. Please use alphabets only.");
        }
    } while (!validateName(newUser.name));

    // DOB input and validation
  do {
        printf("\nEnter date of birth (DD-MM-YYYY): ");
        scanf("%s", newUser.dob);
    if (!validateDOB(newUser.dob)) {
            printf("\nInvalid date format. Please use DD-MM-YYYY format.");
        }
    } while (!validateDOB(newUser.dob));

    // Password input and validation
  do {
        printf("\nCreate a password (min 6 chars): ");
        scanf("%s", newUser.password);
    if (!validatePassword(newUser.password)) {
            printf("\nPassword must be at least 6 characters long.");
        }
    } while (!validatePassword(newUser.password));

    // Phone input and validation
  do {
        printf("\nEnter phone number: ");
        scanf("%s", newUser.phone);
    if (!validatePhone(newUser.phone)) {
            printf("\nPhone number must be 10 digits.");
    } else if (!isPhoneUnique(newUser.phone)) {
            printf("\nPhone number already registered.");
        }
    } while (!validatePhone(newUser.phone) || !isPhoneUnique(newUser.phone));

    // National ID input and validation
  do {
        printf("\nEnter National ID: ");
        scanf("%s", newUser.nationalID);
    if (!validateNationalID(newUser.nationalID)) {
            printf("\nNational ID must be numeric.");
    } else if (!isNationalIDUnique(newUser.nationalID)) {
            printf("\nNational ID already registered.");
        }
  } while (!validateNationalID(newUser.nationalID) ||
           !isNationalIDUnique(newUser.nationalID));

    // Address
    printf("\nEnter Address: ");
    scanf(" %[^\n]", newUser.address);

    // PIN input and validation
  do {
        printf("\nSet a 4-digit PIN: ");
        scanf("%s", newUser.pin);
    if (!validatePIN(newUser.pin)) {
            printf("\nPIN must be 4 digits.");
        }
    } while (!validatePIN(newUser.pin));

    // Initial balance
    newUser.balance = 0.0f;
    newUser.accountNumber = 1000 + userCount; // Simple account number generation

    // Confirmation page loop
  while (1) {
        // Display confirmation page
        system("cls");
        printf("\n\n============================================");
        printf("\nPlease Confirm Your Details");
        printf("\n============================================");
        printf("\nName          : %s", newUser.name);
        printf("\nDOB           : %s", newUser.dob);
        printf("\nPhone         : %s", newUser.phone);
        printf("\nNational ID   : %s", newUser.nationalID);
        printf("\nAddress       : %s", newUser.address);
        printf("\nPassword      : %s", newUser.password);
        printf("\nPIN           : %s", newUser.pin);
        printf("\n============================================");
        printf("\nAre all details correct? (y/n): ");
        scanf(" %c", &confirm);

    if (toupper(confirm) == 'Y') {
            // Save user data and redirect to login
            users[userCount++] = newUser;
            updateUserTxtFile(newUser);
            saveUsersToCSV();

      printf("\nRegistration successful! Your account number is %d",
             newUser.accountNumber);
            printf("\nPress any key to continue to login...");
            getchar();
            getchar();
            loginUser(); // Redirect to login
            break;
    } else if (toupper(confirm) == 'N') {
            // Call the edit function
            editRegistrationFields(&newUser);
            // Continue the loop to show the confirmation page again
    } else {
            printf("\nInvalid choice. Please enter Y or N.");
            printf("\nPress any key to continue...");
            getchar();
            getchar();
        }
    }
}

void loginUser() {
    char phone[11], password[20], pin[5];
    int userIndex = -1;

    system("cls");
    printf("\n\n============================================");
    printf("\nUser Login");
    printf("\n============================================");

    printf("\nEnter your phone number: ");
    scanf("%s", phone);

    printf("\nEnter your password: ");
    scanf("%s", password);

    printf("\nEnter your PIN: ");
    scanf("%s", pin);

    // Find user
  for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].phone, phone) == 0 &&
            strcmp(users[i].password, password) == 0 &&
        strcmp(users[i].pin, pin) == 0) {
            userIndex = i;
            break;
        }
    }

  if (userIndex == -1) {
        printf("\nInvalid credentials...");
        getchar();
        getchar();
        mainMenu();
  } else {
        printf("\nLogin successful!...");
        getchar();
        getchar();
        userDashboard(userIndex);
    }
}

void adminLogin() {
    char id[20], password[20];

    system("cls");
    printf("\n\n============================================");
    printf("\nAdmin Login");
    printf("\n============================================");

    printf("\nEnter admin ID: ");
    scanf("%s", id);

    printf("\nEnter admin password: ");
    scanf("%s", password);

  if (strcmp(id, ADMIN_ID) == 0 && strcmp(password, ADMIN_PASSWORD) == 0) {
        printf("\nAdmin login successful! ");
        getchar();
        getchar();
        adminDashboard();
  } else {
        printf("\nInvalid credentials.");
        getchar();
        getchar();
        mainMenu();
    }
}

void userDashboard(int userIndex) {
    int choice;
    while(1) {
        system("cls");
        printf("\n\n=== USER DASHBOARD ===");
        printf("\n1. Deposit Money");
        printf("\n2. Withdraw Money");
        printf("\n3. Transfer Money");
        printf("\n4. View Account Details");
        printf("\n5. Generate My Statement");
        printf("\n6. View Broadcasts");
        printf("\n7. Contact Admin");
        printf("\n8. Logout");
        printf("\nEnter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input, please enter a number.");
            while (getchar() != '\n');
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
            continue;
        }
        switch(choice) {
            case 1: depositMoney(userIndex); break;
            case 2: withdrawMoney(userIndex); break;
            case 3: transferMoney(userIndex); break;
            case 4: viewAccountDetails(userIndex); break;
            case 5: generateUserStatement(users[userIndex].accountNumber); break;
            case 6: viewBroadcastMessages(); break;
            case 7: contactAdmin(userIndex); break;
            case 8:
            saveUsersToFile();
            saveTransactionsToFile();
            saveMessagesToFile();
            mainMenu();
            return;
        default:
            printf("\nInvalid option, try again");
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
        }
    }
}

void viewAccountDetails(int userIndex) {
    system("cls");
    printf("\n\n============================================");
    printf("\nAccount Details");
    printf("\n============================================");
    printf("\nName: %s", users[userIndex].name);
    printf("\nDOB: %s", users[userIndex].dob);
    printf("\nPhone: %s", users[userIndex].phone);
    printf("\nNational ID: %s", users[userIndex].nationalID);
    printf("\nAddress: %s", users[userIndex].address);
    printf("\nAccount Number: %d", users[userIndex].accountNumber);
    printf("\nBalance: %.2f", users[userIndex].balance);

    // Display recent transactions
    printf("\n\nRecent Transactions:");
    printf("\nDate       Time     Type            Amount");
    printf("\n--------------------------------------------");

    int count = 0;
  for (int i = transactionCount - 1; i >= 0 && count < 5; i--) {
    if (transactions[i].accountNumber == users[userIndex].accountNumber) {
      printf("\n%s %s %-15s %.2f", transactions[i].date, transactions[i].time,
                   transactions[i].type, transactions[i].amount);
      if (transactions[i].targetAccount != -1) {
                printf(" (to %d)", transactions[i].targetAccount);
            }
            count++;
        }
    }

    printf("\n============================================");
    printf("\nPress any key to continue...");
    getchar();
    getchar();
    userDashboard(userIndex);
}

void resetUserPassword(int userIndex) {
    char currentPassword[20], newPassword[20], confirmPassword[20];

    system("cls");
    printf("\n\n============================================");
    printf("\nReset Password");
    printf("\n============================================");

    printf("\nEnter current password: ");
    scanf("%s", currentPassword);

  if (strcmp(currentPassword, users[userIndex].password) != 0) {
        printf("\nCurrent password is incorrect. Press any key to continue...");
        getchar();
        getchar();
        userDashboard(userIndex);
        return;
    }

  do {
        printf("\nEnter new password (min 6 characters): ");
        scanf("%s", newPassword);

    if (!validatePassword(newPassword)) {
            printf("\nPassword must be at least 6 characters long.");
        }
    } while (!validatePassword(newPassword));

    printf("\nConfirm new password: ");
    scanf("%s", confirmPassword);

  if (strcmp(newPassword, confirmPassword) != 0) {
        printf("\nPasswords do not match. Press any key to continue...");
        getchar();
        getchar();
        userDashboard(userIndex);
        return;
    }

    strcpy(users[userIndex].password, newPassword);
  updateUserTxtFile(users[userIndex]);
  saveUsersToCSV();
    printf("\nPassword changed successfully! Press any key to continue...");
    getchar();
    getchar();
    userDashboard(userIndex);
}

void contactAdmin(int userIndex) {
    char message[200];

    system("cls");
    printf("\n\n============================================");
    printf("\nContact Admin");
    printf("\n============================================");

    printf("\nEnter your message (max 200 characters): ");
    scanf(" %[^\n]", message);

    Message msg;
    msg.accountNumber = users[userIndex].accountNumber;
    strcpy(msg.message, message);
    strcpy(msg.reply, "");
    msg.replied = false;

    messages[messageCount++] = msg;
    appendMessageToFile(msg);

    printf("\nMessage sent to admin! Press any key to continue...");
    getchar();
    getchar();
    userDashboard(userIndex);
}

void depositMoney(int userIndex) {
    float amount;
    char date[11], time[9];

    system("cls");
    printf("\n\n============================================");
    printf("\nDeposit Money");
  printf("\nWelcome, %s", users[userIndex].name);
  printf("\n Account: %d", users[userIndex].accountNumber);
  printf("\n Balance: %.2f", users[userIndex].balance);
    printf("\n============================================");

    printf("\nEnter amount to deposit: ");
    scanf("%f", &amount);

  if (amount <= 0) {
        printf("\nAmount must be positive. Press any key to continue...");
        getchar();
        getchar();
        depositMoney(userIndex);
        return;
    }

    users[userIndex].balance += amount;
  updateUserTxtFile(users[userIndex]);
  saveUsersToCSV();

    // Record transaction
    Transaction t;
    t.accountNumber = users[userIndex].accountNumber;
    strcpy(t.type, "Deposit");
    t.amount = amount;
    getCurrentDateTime(t.date, t.time);
    t.targetAccount = -1;

    transactions[transactionCount++] = t;
    appendTransactionToFile(t);

    printf("\nDeposit successful! New balance: %.2f", users[userIndex].balance);
    printf("\nPress any key to continue...");
    getchar();
    getchar();
    userDashboard(userIndex);
}

void withdrawMoney(int userIndex) {
    float amount;
    char pin[5];

    system("cls");
    printf("\n\n============================================");
    printf("\nWithdraw Money");
  printf("\nWelcome, %s", users[userIndex].name);
  printf("\n Account: %d", users[userIndex].accountNumber);
  printf("\n Balance: %.2f", users[userIndex].balance);
    printf("\n============================================");

    printf("\nEnter amount to withdraw: ");
    scanf("%f", &amount);

    printf("\nEnter your PIN: ");
    scanf("%s", pin);

  if (strcmp(pin, users[userIndex].pin) != 0) {
        printf("\nInvalid PIN. Press any key to continue...");
        getchar();
        getchar();
        withdrawMoney(userIndex);
        return;
    }

  if (amount <= 0) {
        printf("\nAmount must be positive. Press any key to continue...");
        getchar();
        getchar();
        withdrawMoney(userIndex);
        return;
    }

  if (amount > users[userIndex].balance) {
        printf("\nBalance too low. Press any key to continue...");
        getchar();
        getchar();
        withdrawMoney(userIndex);
        return;
    }

    users[userIndex].balance -= amount;
  updateUserTxtFile(users[userIndex]);
  saveUsersToCSV();

    // Record transaction
    Transaction t;
    t.accountNumber = users[userIndex].accountNumber;
    strcpy(t.type, "Withdrawal");
    t.amount = amount;
    getCurrentDateTime(t.date, t.time);
    t.targetAccount = -1;

    transactions[transactionCount++] = t;
    appendTransactionToFile(t);

  printf("\nWithdrawal successful! New balance: %.2f",
         users[userIndex].balance);
    printf("\nPress any key to continue...");
    getchar();
    getchar();
    userDashboard(userIndex);
}

void transferMoney(int userIndex) {
    float amount;
    char pin[5];
    int targetAccount, targetIndex = -1;

    system("cls");
    printf("\n\n============================================");
    printf("\nTransfer Money");
  printf("\nWelcome, %s", users[userIndex].name);
  printf("\n Account: %d", users[userIndex].accountNumber);
  printf("\n Balance: %.2f", users[userIndex].balance);
    printf("\n============================================");

    printf("\nEnter target account number: ");
    scanf("%d", &targetAccount);

    // Find target account
    for (int i = 0; i < userCount; i++) {
        if (users[i].accountNumber == targetAccount) {
            targetIndex = i;
            break;
        }
    }

    if (targetIndex == -1) {
        printf("\nTarget account not found. Press any key to continue...");
        getchar();
        getchar();
        transferMoney(userIndex);
        return;
    }

    // Show target account details
    printf("\nTarget Account Found:");
    printf("\nName : %s", users[targetIndex].name);
    printf("\nPhone: %s", users[targetIndex].phone);

    printf("\nEnter amount to transfer: ");
    scanf("%f", &amount);

    printf("\nEnter your PIN: ");
    scanf("%s", pin);

    // Validate PIN
  if (strcmp(pin, users[userIndex].pin) != 0) {
        printf("\nInvalid PIN. Press any key to continue...");
        getchar();
        getchar();
        transferMoney(userIndex);
        return;
    }

    // Validate amount
  if (amount <= 0) {
        printf("\nAmount must be positive. Press any key to continue...");
        getchar();
        getchar();
        transferMoney(userIndex);
        return;
    }

  if (amount > users[userIndex].balance) {
        printf("\nBalance too low. Press any key to continue...");
        getchar();
        getchar();
        transferMoney(userIndex);
        return;
    }

    // Perform transfer
    users[userIndex].balance -= amount;
    users[targetIndex].balance += amount;
  updateUserTxtFile(users[userIndex]);
  updateUserTxtFile(users[targetIndex]);
  saveUsersToCSV();

    // Record transactions for both accounts
    Transaction t1, t2;
    char date[11], time[9];
    getCurrentDateTime(date, time);

    // Sender's transaction
    t1.accountNumber = users[userIndex].accountNumber;
    strcpy(t1.type, "Transfer Out");
    t1.amount = amount;
    strcpy(t1.date, date);
    strcpy(t1.time, time);
    t1.targetAccount = targetAccount;

    // Receiver's transaction
    t2.accountNumber = targetAccount;
    strcpy(t2.type, "Transfer In");
    t2.amount = amount;
    strcpy(t2.date, date);
    strcpy(t2.time, time);
    t2.targetAccount = users[userIndex].accountNumber;

    transactions[transactionCount++] = t1;
    transactions[transactionCount++] = t2;
    appendTransactionToFile(t1);
    appendTransactionToFile(t2);

    printf("\nTransfer successful! New balance: %.2f", users[userIndex].balance);
    printf("\nPress any key to continue...");
    getchar();
    getchar();
    userDashboard(userIndex);
}

void adminDashboard() { 
    int choice;

    system("cls");
    printf("\n\n============================================");
    printf("\n\t\tADMIN DASHBOARD"); 
    printf("\n============================================");
    
    // User Management
    printf("\n1. View All Accounts");
    printf("\n2. Edit User Account Info");
    printf("\n3. Reset User Password");
    printf("\n4. Delete User Account");
    
    // Transaction Management
    printf("\n5. View Transaction Logs");
    printf("\n6. Generate Statement for Any User");
    
    // Communication
    printf("\n7. View and Reply Messages");
    printf("\n8. Broadcast Message to All Users");
    
    // System
    printf("\n9. Logout"); 
    
    printf("\n============================================");
    printf("\nEnter your choice: ");
    scanf("%d", &choice);

    switch(choice) { 
        case 1: viewAllAccounts(); break;
        case 2: editUserAccountInfo(); break;
        case 3: resetUserPasswordAdmin(); break;
        case 4: deleteUserAccount(); break;
        case 5: viewTransactionLogs(); break;
        case 6: generateUserStatement(); break;
        case 7: viewAndReplyMessages(); break;
        case 8: broadcastMessage(); break;
        case 9: 
        saveUsersToFile();
        saveTransactionsToFile();
        saveMessagesToFile();
        mainMenu();
        break;
    default:
        printf("\nInvalid option, try again");
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
        adminDashboard();
    }
}

void viewAllAccounts() {
    char searchTerm[50];
    int found = 0;

    system("cls");
    printf("\n\n============================================");
    printf("\nAll Accounts");
    printf("\n============================================");
  printf(
      "\nEnter search term (name/phone/account number) or 'all' to view all: ");
    scanf("%s", searchTerm);

    printf("\n%-20s %-15s %-15s %-10s", "Name", "Phone", "Account No", "Balance");
    printf("\n--------------------------------------------------");

  for (int i = 0; i < userCount; i++) {
        if (strcmp(searchTerm, "all") == 0 ||
            strstr(users[i].name, searchTerm) != NULL ||
            strstr(users[i].phone, searchTerm) != NULL ||
        atoi(searchTerm) == users[i].accountNumber) {
      printf("\n%-20s %-15s %-15d %-10.2f", users[i].name, users[i].phone,
                   users[i].accountNumber, users[i].balance);
            found++;
        }
    }

  if (!found) {
        printf("\nNo accounts found matching your search.");
    }

    printf("\n============================================");
    printf("\nPress any key to continue...");
    getchar();
    getchar();
    adminDashboard();
}

void deleteUserAccount() {
    int accountNumber;
    char confirm;

    system("cls");
    printf("\n\n============================================");
    printf("\nDelete Account");
    printf("\n============================================");

    printf("\nEnter account number to delete: ");
    scanf("%d", &accountNumber);

    int index = -1;
  for (int i = 0; i < userCount; i++) {
    if (users[i].accountNumber == accountNumber) {
            index = i;
            break;
        }
    }

  if (index == -1) {
        printf("\nAccount not found. Press any key to continue...");
        getchar();
        getchar();
        adminDashboard();
        return;
    }

  printf("\nAccount found: %s (%d)", users[index].name,
         users[index].accountNumber);
    printf("\nAre you sure you want to delete this account? (Y/N): ");
    scanf(" %c", &confirm);

  if (toupper(confirm) == 'Y') {
        // Shift all elements after index to the left
    for (int i = index; i < userCount - 1; i++) {
            users[i] = users[i + 1];
        }
        userCount--;

        printf("\nAccount deleted successfully! Press any key to continue...");
        getchar();
        getchar();
  } else {
        printf("\nAccount deletion cancelled. Press any key to continue...");
        getchar();
        getchar();
    }

    adminDashboard();
}

void resetUserPasswordAdmin() {
    int accountNumber;
    char newPassword[20];

    system("cls");
    printf("\n\n============================================");
    printf("\nReset User Password");
    printf("\n============================================");

    printf("\nEnter account number: ");
    scanf("%d", &accountNumber);

    int index = -1;
  for (int i = 0; i < userCount; i++) {
    if (users[i].accountNumber == accountNumber) {
            index = i;
            break;
        }
    }

  if (index == -1) {
        printf("\nAccount not found. Press any key to continue...");
        getchar();
        getchar();
        adminDashboard();
        return;
    }

  do {
        printf("\nEnter new password (min 6 characters): ");
        scanf("%s", newPassword);

    if (!validatePassword(newPassword)) {
            printf("\nPassword must be at least 6 characters long.");
        }
    } while (!validatePassword(newPassword));

    strcpy(users[index].password, newPassword);
    printf("\nPassword reset successfully! Press any key to continue...");
    getchar();
    getchar();
    adminDashboard();
}

void viewAndReplyMessages() {
    char reply[200];

    system("cls");
    printf("\n\n============================================");
    printf("\nUser Messages");
    printf("\n============================================");

  if (messageCount == 0) {
        printf("\nNo messages to display.");
  } else {
    for (int i = 0; i < messageCount; i++) {
            printf("\nMessage from account %d:", messages[i].accountNumber);
            printf("\nMessage: %s", messages[i].message);

      if (messages[i].replied) {
                printf("\nReply: %s", messages[i].reply);
      } else {
                printf("\nNo reply yet.");
                printf("\nEnter reply (or 'skip' to skip): ");
                scanf(" %[^\n]", reply);

        if (strcmp(reply, "skip") != 0) {
                    strcpy(messages[i].reply, reply);
                    messages[i].replied = true;
                    printf("\nReply sent!");
                }
            }
            printf("\n--------------------------------------------");
        }
    }

    printf("\n============================================");
    printf("\nPress any key to continue...");
    getchar();
    getchar();
    adminDashboard();
}

void viewTransactionLogs() {
    system("cls");
    printf("\n\n============================================");
    printf("\n\tTRANSACTION LOGS");
    printf("\n============================================");
    
    FILE *csvFile = fopen("transactions.csv", "r");
    if(csvFile == NULL) {
        printf("\nNo transactions found.");
        printf("\nPress any key to continue...");
        getchar(); getchar();
        return;
    }

    char line[256];
    printf("\n%-12s %-10s %-8s %-15s %-10s %-15s", 
           "Account", "Date", "Time", "Type", "Amount", "Target Account");
    printf("\n------------------------------------------------------------");
    
    fgets(line, sizeof(line), csvFile); // Skip header
    while(fgets(line, sizeof(line), csvFile)) {
        Transaction t;
        int n = sscanf(line, "%d,%10[^,],%8[^,],%14[^,],%f,%d",
              &t.accountNumber, t.date, t.time, t.type, &t.amount, &t.targetAccount);
        
        printf("\n%-12d %-10s %-8s %-15s %-10.2f", 
               t.accountNumber, t.date, t.time, t.type, t.amount);
        if(n == 6 && t.targetAccount != -1) {
            printf(" %-15d", t.targetAccount);
        } else {
            printf(" %-15s", "N/A");
        }
    }
    fclose(csvFile);
    
    printf("\n\nPress any key to continue...");
    getchar(); getchar();
}

void broadcastMessage() {
    char message[200];
    system("cls");
    printf("\n\n============================================");
    printf("\n\tBROADCAST MESSAGE TO ALL USERS");
    printf("\n============================================");
    
    printf("\nEnter your message (max 200 chars): ");
    scanf(" %[^\n]", message);
    
    // Create broadcast message file
    FILE *broadcast = fopen("broadcast.txt", "w");
    if(broadcast) {
        fprintf(broadcast, "%s", message);
        fclose(broadcast);
        printf("\nMessage broadcasted to all users!");
    } else {
        printf("\nError creating broadcast message!");
    }
    
    printf("\nPress any key to continue...");
    getchar(); getchar();
}

void editUserAccountInfo() {
    if(!isAdminLoggedIn()) {
        printf("Access denied!");
        return;
    }
    int accNumber, found = 0;
    system("cls");
    printf("\n\n============================================");
    printf("\n\tEDIT USER ACCOUNT INFORMATION");
    printf("\n============================================");
    printf("\nEnter account number to edit: ");
    scanf("%d", &accNumber);
    for(int i = 0; i < userCount; i++) {
        if(users[i].accountNumber == accNumber) {
            found = 1;
            printf("\nEditing account: %s (%d)", users[i].name, users[i].accountNumber);
            editRegistrationFields(&users[i]);
            updateUserTxtFile(users[i]);
            saveUsersToCSV();
            break;
        }
    }
    if(!found) {
        printf("\nAccount not found!");
    }
    printf("\nPress any key to continue...");
    getchar(); getchar();
}

void generateUserStatement() {
    int accNumber, month, year;
    system("cls");
    printf("\n\n============================================");
    printf("\n\tGENERATE USER STATEMENT");
    printf("\n============================================");
    
    printf("\nEnter account number: ");
    scanf("%d", &accNumber);
    printf("Enter month (1-12): ");
    scanf("%d", &month);
    printf("Enter year: ");
    scanf("%d", &year);
    
    // Find user
    int userIndex = -1;
    for(int i = 0; i < userCount; i++) {
        if(users[i].accountNumber == accNumber) {
            userIndex = i;
            break;
        }
    }
    
    if(userIndex == -1) {
        printf("\nAccount not found!");
        printf("\nPress any key to continue...");
        getchar(); getchar();
        return;
    }
    
    // Generate statement
    char filename[50];
    sprintf(filename, "statement_%d_%02d_%04d.txt", accNumber, month, year);
    FILE *stmt = fopen(filename, "w");
    
    if(stmt) {
        fprintf(stmt, "BANK STATEMENT\n");
        fprintf(stmt, "Account: %d\n", users[userIndex].accountNumber);
        fprintf(stmt, "Name: %s\n", users[userIndex].name);
        fprintf(stmt, "Period: %02d/%04d\n\n", month, year);
        fprintf(stmt, "DATE\t\tTYPE\t\tAMOUNT\t\tBALANCE\n");
        fprintf(stmt, "------------------------------------------------\n");
        
        displayTransactions(accNumber, month, year);
        
        fprintf(stmt, "\nClosing Balance: %.2f", users[userIndex].balance);
        fclose(stmt);
        
        printf("\nStatement generated as %s", filename);
    } else {
        printf("\nError generating statement!");
    }
    
    printf("\nPress any key to continue...");
    getchar(); getchar();
}

void viewBroadcastMessages() {
    FILE *broadcast = fopen("broadcast.txt", "r");
    if(broadcast) {
        printf("\n=== LATEST BROADCAST ===\n");
        char line[200];
        while(fgets(line, sizeof(line), broadcast)) {
            printf("%s", line);
        }
        fclose(broadcast);
    } else {
        printf("\nNo broadcast messages available.");
    }
    printf("\nPress any key to continue...");
    getchar(); getchar();
}

void generateMyStatement(int userIndex) {
    int month, year;
    system("cls");
    printf("\n\n============================================");
    printf("\n\tGENERATE MY STATEMENT");
    printf("\n============================================");
    printf("\nEnter month (1-12): ");
    scanf("%d", &month);
    printf("Enter year: ");
    scanf("%d", &year);

    char filename[64];
    sprintf(filename, "statement_%d_%02d_%04d.txt", users[userIndex].accountNumber, month, year);
    FILE *stmt = fopen(filename, "w");
    if (stmt) {
        fprintf(stmt, "BANK STATEMENT\n");
        fprintf(stmt, "Account: %d\n", users[userIndex].accountNumber);
        fprintf(stmt, "Name: %s\n", users[userIndex].name);
        fprintf(stmt, "Period: %02d/%04d\n\n", month, year);
        fprintf(stmt, "DATE\t\tTYPE\t\tAMOUNT\t\tBALANCE\n");
        fprintf(stmt, "------------------------------------------------\n");
        displayTransactions(users[userIndex].accountNumber, month, year);
        fprintf(stmt, "\nClosing Balance: %.2f", users[userIndex].balance);
        fclose(stmt);
        printf("\nStatement generated as %s", filename);
    } else {
        printf("\nError generating statement!");
    }
    printf("\nPress any key to continue...");
    getchar(); getchar();
}

void displayTransactions(int accountNumber, int month, int year) {
    FILE *csvFile = fopen("transactions.csv", "r");
    if (csvFile == NULL) {
        printf("\nNo transactions found.");
        return;
    }
    char line[256];
    fgets(line, sizeof(line), csvFile); // Skip header
    printf("\n%-10s %-8s %-15s %-10s %-15s", "Date", "Time", "Type", "Amount", "Target Account");
    printf("\n--------------------------------------------------------------");
    int found = 0;
    while (fgets(line, sizeof(line), csvFile)) {
        int accNum, tAcc = -1;
        char date[11], time[9], type[20];
        float amount;
        int n = sscanf(line, "%d,%10[^,],%8[^,],%19[^,],%f,%d", &accNum, date, time, type, &amount, &tAcc);
        if (accNum == accountNumber) {
            int tMonth, tYear;
            // Parse date as DD-MM-YYYY
            if (sscanf(date, "%*2d-%2d-%4d", &tMonth, &tYear) == 2) {
                if (tMonth == month && tYear == year) {
                    printf("\n%-10s %-8s %-15s %-10.2f ", date, time, type, amount);
                    if (n == 6 && tAcc != -1) {
                        printf("%-15d", tAcc);
                    } else {
                        printf("%-15s", "N/A");
                    }
                    found = 1;
                }
            }
        }
    }
    fclose(csvFile);
    if (!found) {
        printf("\nNo transactions for this period.");
    }
}

void logAdminAction(char *action) {
    FILE *log = fopen("admin_log.txt", "a");
    if(log) {
        char date[20], timeStr[20];
        getCurrentDateTime(date, timeStr);
        fprintf(log, "[%s %s] %s\n", date, timeStr, action);
        fclose(log);
    }
}

bool isAdminLoggedIn() {
    return true;
}

int main() {
    loadUsersFromCSV();
    loadTransactionsFromFile();
    loadMessagesFromFile();
    initTransactionCSV();
    ensureDataFolder();
    ensureUsersTxtFolder();
    mainMenu();
    return 0;
}