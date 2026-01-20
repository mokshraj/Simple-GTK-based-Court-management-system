#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
int creating_database()
{
    sqlite3 *db;
    char *errmsg;
    int open, create1, create2, create3, create4, create5,create6,create7,create8;
    open = sqlite3_open("Judgment.db", &db);
    if (open != SQLITE_OK)
    {
        printf("File not opened!\n");
        return 0;
    }
    sqlite3_exec(db, "PRAGMA foreign keys = ON", NULL, NULL, NULL);
    const char *create_table1 = "CREATE TABLE IF NOT EXISTS Judge("
                                "Judge_ID TEXT PRIMARY KEY ,"
                                "Name TEXT NOT NULL,"
                                "Age INT,"
                                "Address TEXT NOT NULL,"
                                "Contact_Number TEXT ,"
                                "Qualification TEXT NOT NULL,"
                                "Experience INT NOT NULL,"
                                "Complete_cases INT ,"
                                "status TEXT,"
                                "court_level TEXT,"
                                "Password TEXT NOT NULL);";
    create1 = sqlite3_exec(db, create_table1, NULL, NULL, NULL);
    if (create1 != SQLITE_OK)
    {
        fprintf(stderr, " JUDGE table not created ! : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    else
    {
        printf(" JUDGE table created\n");
    }
    const char *create_table2 = "CREATE TABLE IF NOT EXISTS Lawyer("
                                "Lyr_ID TEXT PRIMARY KEY,"
                                "Name TEXT NOT NULL,"
                                "Age INT,"
                                "Address TEXT NOT NULL,"
                                "Contact_Number TEXT,"
                                "Qualification TEXT NOT NULL,"
                                "Experience INT NOT NULL,"
                                "Total_Case INT NOT NULL,"
                                "Won INT,"
                                "status TEXT,"
                                "court_level TEXT,"
                                "Password TEXT NOT NULL);";
    create2 = sqlite3_exec(db, create_table2, NULL, NULL, &errmsg);
    if (create2 != SQLITE_OK)
    {
        fprintf(stderr, "LAWYER table is not created : %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return 0;
    }
    else
    {
        printf("\n LAWYER table is created \n");
    }
    const char *create_table3 = "CREATE TABLE IF NOT EXISTS Criminals("
                                "Criminal_ID TEXT PRIMARY KEY,"
                                "FIR_NUMBER INT NOT NULL,"
                                "Name TEXT NOT NULL,"
                                "Age INT,"
                                "Address TEXT,"
                                "Crime_date DATE DEFAULT CURRENT_DATE,"
                                "Incident_Place TEXT NOT NULL,"
                                "IPC_Section INT NOT NULL,"
                                "Crime TEXT NOT NULL,"
                                "satus TEXT NOT  NULL,"
                                "Punishment TEXT NOT NULL,"
                                "court_level TEXT NOT NULL,"
                                "Police_holder_name TEXT);";
    create3 = sqlite3_exec(db, create_table3, NULL, NULL, NULL);
    if (create3 != SQLITE_OK)
    {
        fprintf(stderr, "CRIMINALS table not created : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    else
    {
        printf("CRIMINALS table created !\n");
    }
    const char *create_table4 = "CREATE TABLE IF NOT EXISTS Crime_table("
                                "Case_ID TEXT,"
                                "FIR_Number INTEGER PRIMARY KEY,"
                                "Name TEXT NOT NULL,"
                                "Judge_ID TEXT,"
                                "Lyr1_ID TEXT,"
                                "Lyr2_ID TEXT,"
                                "Criminal_ID TEXT,"
                                "Case_handler Text NOT NULL,"
                                "status TEXT,"
                                "Punishment TEXT,"
                                "Verdict TEXT,"
                                "Crime_date DATE DEFAULT CURRENT_DATE,"
                                "Incident_Place TEXT NOT NULL,"
                                "court_level TEXT,"
                                "FOREIGN KEY(Judge_ID) REFERENCES Judge(Judge_ID),"
                                "FOREIGN KEY(Criminal_ID) REFERENCES Criminals(Criminal_ID));";

    create4 = sqlite3_exec(db, create_table4, NULL, NULL, NULL);
    if (create4 != SQLITE_OK)
    {
        fprintf(stderr, "\nCRIME table is not created : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    else
    {
        printf("\nCRIME table created !\n");
    }
    const char *create_table5 = "CREATE TABLE IF NOT EXISTS citizen("
                                "User_ID TEXT PRIMARY KEY,"
                                "Name TEXT NOT NULL,"
                                "Age INT NOT NULL,"
                                "Address TEXT NOT NULL,"
                                "Contact_Number TEXT,"
                                "Password TEXT NOT NULL);";
    create5 = sqlite3_exec(db, create_table5, NULL, NULL, NULL);
    if (create5 != SQLITE_OK)
    {
        fprintf(stderr, "CITIZEN table is not created : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    else
    {
        printf("Citizen table created !\n");
    }
    const char *create_table6 = "CREATE TABLE IF NOT EXISTS clerk("
                                "User_ID TEXT PRIMARY KEY,"
                                "Name TEXT NOT NULL,"
                                "Age INT NOT NULL,"
                                "Address TEXT NOT NULL,"
                                "Contact_Number TEXT,"
                                "Joined_date DATE DEFAULT CURRENT_DATE,"
                                "Password TEXT NOT NULL);";
    create6 = sqlite3_exec(db, create_table6, NULL, NULL, NULL);
    if (create6 != SQLITE_OK)
    {
        fprintf(stderr, "Citizen table is not created : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    else
    {
        printf("CLERK table created !\n");
    }
    const char *create_table7 = "CREATE TABLE IF NOT EXISTS Evidence("
                                "Case_ID TEXT PRIMARY KEY,"
                                "Criminal_Name TEXT NOT NULL,"
                                "Age INT NOT NULL,"
                                "Evidence TEXT NOT NULL,"
                                "Address TEXT NOT NULL,"
                                "Contact_Number TEXT);";
    create7 = sqlite3_exec(db, create_table7, NULL, NULL, NULL);
    if (create7 != SQLITE_OK)
    {
        fprintf(stderr, "EVIDENCE table is not created : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    else
    {
        printf("EVIDENCE table created !\n");
    }
    const char *create_table8 = "CREATE TABLE IF NOT EXISTS case_schedule ("
    "schedule_id INTEGER,"
    "Case_id INTEGER NOT NULL," 
    //"judge_id INTEGER NOT NULL," // Removed
    "clerk_id INTEGER,"
    "courtroom_no TEXT,"
    "date DATE NOT NULL,"
    "start_time TIME NOT NULL,"
    "end_time TIME,"
    "status TEXT DEFAULT 'Scheduled',"
    "remarks TEXT,"
    "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
    "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
    "Video_id TEXT," // Added
    "Proceding_time TEXT," // Added
    "Break_time TEXT," // Added
    "PRIMARY KEY(schedule_id AUTOINCREMENT));"; 
    
    create8=sqlite3_exec(db,create_table6,NULL,NULL,NULL);
    if(create8!=SQLITE_OK)
    {
        fprintf(stderr,"case_schedule table is not created : %s\n",sqlite3_errmsg(db));
    }
    else
    {
        printf("case_schedule table created !\n");
    }


    sqlite3_close(db);
    return 1;
}
int Judge_registration()
{

    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare, insert;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("Database not responding !\n");
        return 0;
    }
    const char *sql_insert = "INSERT OR IGNORE INTO Judge(Judge_ID,Name,Age,Address,Contact_Number,Qualification,Experience,Complete_cases,status,court_level,Password)"
                             "VALUES(?,?,?,?,?,?,?,?,?,?,?);";
    prepare = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Compilation of Insert command facing issue !\n");
        sqlite3_close(db);
        return 0;
    }
    int age, experience, complete_cases;
    char name[20], Address[50], contact_number[15], Qualification[40], status[15], court_level[15], password[20], judge_id[20];
    printf("Enter Id :\n");
    scanf(" %[^\n]", judge_id);
    printf("Enter name : \n");
    scanf(" %[^\n]", name);
    printf("Enter  Address :\n");
    scanf(" %[^\n]", Address);
    printf("Enter contact number :\n");
    scanf(" %[^\n]", contact_number);
    printf("Enter Qualification details :\n");
    scanf(" %[^\n]", Qualification);
    printf("Enter alive or not :\n");
    scanf(" %[^\n]", status);
    printf("Enter level of court :\n");
    scanf(" %[^\n]", court_level);
    printf("Kindly enter password :\n");
    scanf(" %[^\n]", password);
    printf("Enter Age : \n");
    scanf("%d", &age);
    printf("Enter Experience in years  :\n");
    scanf("%d", &experience);
    printf("Enter Complete cases in numbers :\n");
    scanf("%d", &complete_cases);

    sqlite3_bind_text(stmt, 1, judge_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, age);
    sqlite3_bind_text(stmt, 4, Address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, contact_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, Qualification, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, experience);
    sqlite3_bind_int(stmt, 8, complete_cases);
    sqlite3_bind_text(stmt, 9, status, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 10, court_level, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 11, password, -1, SQLITE_STATIC);
    insert = sqlite3_step(stmt);
    if (insert != SQLITE_DONE)
    {
        fprintf(stderr, "Data failed to insert: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("data loaded in DB !");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

int Judge_login(char user_id[20])
{

    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("Database not responding !\n");
        return 0;
    }
    const char *retrieve_data = "SELECT Password FROM Judge WHERE Judge_ID = ?";
    prepare = sqlite3_prepare_v2(db, retrieve_data, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Parsing and compiling problem... !\n");
        return 0;
    }

    char passwd[20];
    int data, found = 0;
    printf("Password : ");
    scanf(" %[^\n]", passwd);
    sqlite3_bind_text(stmt, 1, user_id, -1, NULL);
    data = sqlite3_step(stmt);

    if (data == SQLITE_ROW)
    {
        const unsigned char *db_password = sqlite3_column_text(stmt, 0);
        if (strcmp(passwd, (const char *)db_password) == 0)
        {
            printf("\nLogin successfull !\n");
            found = 1;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return found;
        }
        else
        {
            printf("Incorrect password !\n");
        }
    }
    else
    {
        if (data == SQLITE_DONE)
        {
            printf("user id not found !\n");
        }
        else
        {
            printf("Error :\n", sqlite3_errmsg(db));
        }
    }
    if (!found)
    {
        printf("Either User_id , password invalid or may this account not exist !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return found;
}
int lawyer_registration()
{

    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare, insert;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("Database not responding !\n");
        return 0;
    }
    const char *sql_insert = "INSERT OR IGNORE INTO Lawyer(Lyr_ID,Name,Age,Address,Contact_Number,Qualification,Experience,Total_case,Won,status,court_level,Password)"
                             "VALUES(?,?,?,?,?,?,?,?,?,?,?,?);";
    prepare = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Compilation of Insert command facing issue !\n");
        sqlite3_close(db);
        return 0;
    }
    int age, experience, Total_case, won;
    char name[20], Address[50], contact_number[13], Qualification[40], status[11], court_level[15], password[6], Lyr_id[20];
    printf("Enter Id :\n");
    scanf(" %[^\n]", Lyr_id);
    printf("Enter name : \n");
    scanf(" %[^\n]", name);
    printf("Enter contact number :\n");
    scanf(" %[^\n]", contact_number);
    printf("Enter  Address :\n");
    scanf(" %[^\n]", Address);
    printf("Enter Qualification details :\n");
    scanf(" %[^\n]", Qualification);
    printf("Enter alive or not :\n");
    scanf(" %[^\n]", status);
    printf("Enter level of court :\n");
    scanf(" %[^\n]", court_level);
    printf("Kindly enter password :\n");
    scanf(" %[^\n]", password);
    printf("Enter Age : \n");
    scanf("%d", &age);
    printf("Enter Experience in years  :\n");
    scanf("%d", &experience);
    printf("Enter Complete cases in numbers :\n");
    scanf("%d", &Total_case);
    printf("How many won out of %d ?", Total_case);
    scanf("%d", &won);
    sqlite3_bind_text(stmt, 1, Lyr_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, age);
    sqlite3_bind_text(stmt, 4, Address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, contact_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, Qualification, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, experience);
    sqlite3_bind_int(stmt, 8, Total_case);
    sqlite3_bind_int(stmt, 9, won);
    sqlite3_bind_text(stmt, 10, status, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 11, court_level, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 12, password, -1, SQLITE_STATIC);
    insert = sqlite3_step(stmt);
    if (insert != SQLITE_DONE)
    {
        fprintf(stderr, "Data failed to insert: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("data loaded in DB !");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}
int Lawyer_login(char lawyer_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("Database not responding !\n");
        return 0;
    }
    const char *retrieve_data = "SELECT Password from Lawyer WHERE Lyr_ID=?";
    prepare = sqlite3_prepare_v2(db, retrieve_data, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Server problem !\n");
        sqlite3_close(db);
        return 0;
    }
    
    char passwd[20];
    int data, found = 0;
    printf("Password :");
    scanf(" %[^\n]", passwd);
    sqlite3_bind_text(stmt,1,lawyer_id,-1,NULL);
    data = sqlite3_step(stmt);
    if (data == SQLITE_ROW)
    {
        const unsigned char *password = sqlite3_column_text(stmt, 0);
        if (strcmp(passwd, (const char *)password) == 0)
        {
            printf("\nLogin successfull !\n");
            found = 1;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return found;
        }
        else
        {
            printf("Incorrect password !\n");
        }
    }
    else
    {
        if (data == SQLITE_DONE)
        {
            printf("\nIncorrect user id\n");
        }
        else
        {
            printf("Error :\n", sqlite3_errmsg(db));
        }
    }
    if (!found)
    {
        printf("Either User_id , password invalid or may this account not exist !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

int citizen_registration()
{

    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare, insert;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("Database not responding !\n");
        return 0;
    }
    const char *sql_insert = "INSERT OR IGNORE INTO citizen(User_ID,Name,Age,Address,Contact_Number,Password)"
                             "VALUES(?,?,?,?,?,?);";
    prepare = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Compilation of Insert command facing issue !\n");
        sqlite3_close(db);
        return 0;
    }
    int age;
    char name[20], Address[50], contact_number[13], password[20], user_id[20];
    printf("Enter Id :\n");
    scanf(" %[^\n]", user_id);
    printf("Enter name : \n");
    scanf(" %[^\n]", name);
    printf("Enter contact number :\n");
    scanf(" %[^\n]", contact_number);
    printf("Enter  Address :\n");
    scanf(" %[^\n]", Address);
    printf("Kindly enter password :\n");
    scanf(" %[^\n]", password);
    printf("Enter Age : \n");
    scanf("%d", &age);
    sqlite3_bind_text(stmt, 1, user_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, age);
    sqlite3_bind_text(stmt, 4, Address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, contact_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, password, -1, SQLITE_STATIC);
    insert = sqlite3_step(stmt);
    if (insert != SQLITE_DONE)
    {
        fprintf(stderr, "Data failed to insert: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("data loaded in DB !");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}
int Citizen_login(char citizen_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("Database not responding !\n");
        return 0;
    }
    const char *retrieve_data = "SELECT Password from citizen WHERE User_ID=?";
    prepare = sqlite3_prepare_v2(db, retrieve_data, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Server problem !\n");
        return 0;
    }

    char passwd[20];
    int data, found = 0;
    printf("Password : ");
    scanf(" %[^\n]", passwd);
    sqlite3_bind_text(stmt,1,citizen_id,-1,NULL);
    data = sqlite3_step(stmt);
    if (data == SQLITE_ROW)
    {
        const unsigned char *password = sqlite3_column_text(stmt, 0);
        if (data == SQLITE_ROW)
        {
            printf("\nLogin successfull !\n");
            found = 1;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return found;
        }
        else
        {
            printf("Incorrect password !\n");
        }
    }
    else
    {
        if (data == SQLITE_DONE)
        {
            printf("User id not found !\n");
        }
        else
        {
            printf("Error :\n", sqlite3_errmsg(db));
        }
    }
    if (!found)
    {
        printf("Either User_id , password invalid or may this account not exist !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}
int clerk_registration(  int age,char name[20],char Address[50],char contact_number[13],char password[20],char user_id[20],char joined_date[15])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare, insert;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("Database not responding !\n");
        return 0;
    }
    const char *sql_insert = "INSERT OR IGNORE INTO clerk(User_ID,Name,Age,Address,Contact_Number,Joined_date,Password)"
                             "VALUES(?,?,?,?,?,?,?);";
    prepare = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Compilation of Insert command facing issue !: %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_text(stmt, 1, user_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, age);
    sqlite3_bind_text(stmt, 4, Address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, contact_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, joined_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, password, -1, SQLITE_STATIC);
    insert = sqlite3_step(stmt);
    if (insert != SQLITE_DONE)
    {
        fprintf(stderr, "Data failed to insert: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("data loaded in DB !");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;

    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;

}
int clerk_login(char clerk_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("Database not responding !\n");
        return 0;
    }
    const char *retrieve_data = "SELECT Password from clerk WHERE User_ID=?";
    prepare = sqlite3_prepare_v2(db, retrieve_data, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Server problem !\n");
        return 0;
    }

    char passwd[20];
    int data, found = 0;
    printf("Password : ");
    scanf(" %[^\n]", passwd);
    sqlite3_bind_text(stmt,1,clerk_id,-1,NULL);
    data = sqlite3_step(stmt);
    if (data == SQLITE_ROW)
    {
        const unsigned char *password = sqlite3_column_text(stmt, 0);
        if (data == SQLITE_ROW)
        {
            printf("\nLogin successful !\n");
            found = 1;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return found;
        }
        else
        {
            printf("Incorrect password !\n");
        }
    }
    else
    {
        if (data == SQLITE_DONE)
        {
            printf("User id not found !\n");
        }
        else
        {
            printf("Error :\n", sqlite3_errmsg(db));
        }
    }
    if (!found)
    {
        printf("Either User_id , password invalid or may this account not exist !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}
int criminals_entry(char criminal_id[20],long long int Fir_number,char name[20],int age,char Address[50],char crime_date[13],char Incident_place[40],char IPC_section[20],char crime[50],char status[20],char punishment[20],char court_level[20],char police_name[20])
{

    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare, insert;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("Database not responding !\n");
        return 0;
    }
    const char *sql_insert = "INSERT OR IGNORE INTO Criminals(Criminal_ID,FIR_NUMBER,Name,Age,Address,Crime_date,Incident_Place,IPC_Section,Crime,satus,Punishment,court_level,Police_holder_name)"
                             "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?);";
    prepare = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Compilation of Insert command facing issue ! : %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    
    sqlite3_bind_text(stmt, 1, criminal_id, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2,(int)Fir_number);
    sqlite3_bind_text(stmt, 3, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, age);
    sqlite3_bind_text(stmt, 5, Address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, crime_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, Incident_place, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, IPC_section, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, crime, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 10, status, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 11, punishment, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 12, court_level, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 13, police_name, -1, SQLITE_STATIC);
    insert = sqlite3_step(stmt);
    if (insert != SQLITE_DONE)
    {
        fprintf(stderr, "Data failed to insert: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("data loaded in DB !");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

int crime_table_entry(char case_id[20],long long int FIR_number,char name[30],char judge_id[20],char lyr1_id[20],char lyr2_id[20],char criminal_id[20],char case_handler[30],char verdict[13],char punishment[40],char status[11],char crime_date[11],char incident_place[50],char court_level[20])
{

    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare, insert;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("Database not responding !\n");
        return 0;
    }
    const char *sql_insert = "INSERT OR IGNORE INTO Crime_table(Case_ID,FIR_Number,Name,Judge_ID,Lyr1_ID,Lyr2_ID,Criminal_ID,Case_handler,Status,Punishment,Verdict,Crime_date,Incident_Place,court_level)"
                             "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
    prepare = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Compilation of Insert command facing issue ! : %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    
    sqlite3_bind_text(stmt, 1, case_id, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, (int)FIR_number);
    sqlite3_bind_text(stmt, 3, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, judge_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, lyr1_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, lyr2_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, criminal_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, case_handler, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, status, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 10, punishment, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 11, verdict, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 12, crime_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 13, incident_place, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 14, court_level, -1, SQLITE_STATIC);
    insert = sqlite3_step(stmt);
    if (insert != SQLITE_DONE)
    {
        fprintf(stderr, "Data failed to insert: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("data loaded in DB !");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}
void evidence_entry(char case_id[20],char criminal_name[20],int age,char evidence[200],char address[20],char contact_number[16])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare, insert;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("Database not responding !\n");
        return ;
    }
    const char *sql_insert = "INSERT OR IGNORE INTO Evidence(Case_ID,Criminal_Name,Age,Evidence,Address,Contact_Number)"
                             "VALUES(?,?,?,?,?,?);";
    prepare = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Compilation of Insert command facing issue ! : %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }
    sqlite3_bind_text(stmt, 1, case_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, criminal_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, age);
    sqlite3_bind_text(stmt, 4, evidence, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6,contact_number, -1, SQLITE_STATIC);

    insert = sqlite3_step(stmt);
    if (insert != SQLITE_DONE)
    {
        fprintf(stderr, "Data failed to insert: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("data loaded in DB !");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
int update_string_prepare1_v3(char column_name[30], char new_name[50], char judge_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int prepare, rc;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    char update_sql[200];
    sprintf(update_sql, "UPDATE Judge SET %s=? WHERE Judge_ID=?", column_name);
    prepare = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("\nParses faiing try again later !\n");
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_text(stmt, 1, new_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, judge_id,-1,SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("\nUpdate failed : %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("\nUpdate successful !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
int update_integer_prepare1_v3(char column_name[30], int new_value, char judge_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int prepare, rc;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    char update_sql[200];
    sprintf(update_sql, "UPDATE Judge SET %s=? WHERE Judge_ID=?", column_name);
    prepare = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("\nParses faiing try again later !\n");
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_int(stmt, 1, new_value);
    sqlite3_bind_text(stmt, 2, judge_id,-1,SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("\nUpdate failed : %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("\nUpdate successful !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}

void update_judge_information(char judge_id[20])
{
    int col;
    char column_name[30];
    while (1)
    {
        printf("What do you want to update ?\n");
        printf("1)Name\n2)Age\n3)Address\n4)Contact Number\n5)Qualification\n6)Experience\n7)Complete Cases\n8)Status\n9)Court Level\n10)Password\n11)Exit\n");
        scanf("%d", &col);
        if (col == 11)
        {
            break;
        }
        switch (col)
        {
        case 1:
            char updated_name[50];
            strcpy(column_name, "Name");
            printf("Enter updated name :\n");
            scanf(" [%^\n]", updated_name);
            update_string_prepare1_v3(column_name, updated_name, judge_id);
            break;
        case 2:
            int updated_age;
            strcpy(column_name, "Age");
            printf("Enter updated age :\n");
            scanf("%d", &updated_age);
            update_integer_prepare1_v3(column_name, updated_age,judge_id);
            break;
        case 3:
            char updated_address[50];
            strcpy(column_name, "Address");
            printf("Enter updated Address :\n");
            scanf(" %[^\n]", updated_address);
            update_string_prepare1_v3(column_name, updated_address, judge_id);
            break;
        case 4:
            char updated_contact[50];
            strcpy(column_name, "Contact_Number");
            printf("Enter updated Contact Number :\n");
            scanf(" %[^\n]", updated_contact);
            update_string_prepare1_v3(column_name, updated_contact, judge_id);
            break;
        case 5:
            char updated_qualification[50];
            strcpy(column_name, "Qualification");
            printf("Enter updated Qualification details :\n");
            scanf(" %[^\n]", updated_qualification);
            update_string_prepare1_v3(column_name, updated_qualification, judge_id);
            break;
        case 6:
            int updated_experience;
            strcpy(column_name, "Experience");
            printf("Enter updated experience :\n");
            scanf("%d", &updated_experience);
            update_integer_prepare1_v3(column_name, updated_experience, judge_id);
            break;
        case 7:
            int updated_com_case;
            strcpy(column_name, "Complete_cases");
            printf("Enter updated complete cases :\n");
            scanf("%d", &updated_com_case);
            update_integer_prepare1_v3(column_name, updated_com_case, judge_id);
            break;
        case 8:
            char updated_status[50];
            strcpy(column_name, "status");
            printf("Enter updated status :\n");
            scanf(" %[^\n]", updated_status);
            update_string_prepare1_v3(column_name, updated_status, judge_id);
            break;
        case 9:
            char updated_court_level[50];
            strcpy(column_name, "court_level");
            printf("Enter updated court level :\n");
            scanf(" %[^\n]", updated_court_level);
            update_string_prepare1_v3(column_name, updated_court_level, judge_id);
            break;
        case 10:
            char updated_password[50];
            strcpy(column_name, "Password");
            printf("Enter updated password :\n");
            scanf(" %[^\n]", updated_password);
            update_string_prepare1_v3(column_name, updated_password, judge_id);
            break;
        default:
            printf("\nInvalid choice\n");
        }
    }
}
int update_string_prepare2_v3(char column_name[30], char new_name[50], char lawyer_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int prepare, rc;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    char update_sql[200];
    sprintf(update_sql, "UPDATE Lawyer SET %s=? WHERE Lyr_ID=?", column_name);
    prepare = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("\nParses faiing try again later !\n");
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_text(stmt, 1, new_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, lawyer_id,-1,SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("\nUpdate failed : %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("\nUpdate successful !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
int update_integer_prepare2_v3(char column_name[30], int new_value, char lawyer_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int prepare, rc;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    char update_sql[200];
    sprintf(update_sql, "UPDATE Lawyer SET %s=? WHERE Lyr_ID=?", column_name);
    prepare = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("\nParses faiing try again later !\n");
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_int(stmt, 1, new_value);
    sqlite3_bind_text(stmt, 2, lawyer_id,-1,SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("\nUpdate failed : %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("\nUpdate successful !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
void update_lawyer_information(char lawyer_id[20])
{
    int col;
    char column_name[30];
    while (1)
    {
        printf("What do you want to update ?\n");
        printf("1)Name\n2)Age\n3)Address\n4)Contact Number\n5)Qualification\n6)Experience\n7)Total Cases\n8)Status\n9)Court Level\n10)Password\n11)Exit\n");
        scanf("%d", &col);
        if (col == 11)
        {
            break;
        }
        switch (col)
        {
        case 1:
            char updated_name[50];
            strcpy(column_name, "Name");
            printf("Enter updated name :\n");
            scanf(" %[^\n]", updated_name);
            update_string_prepare2_v3(column_name, updated_name, lawyer_id);
            break;
        case 2:
            int updated_age;
            strcpy(column_name, "Age");
            printf("Enter updated age :\n");
            scanf("%d", &updated_age);
            update_integer_prepare2_v3(column_name, updated_age,lawyer_id);
            break;
        case 3:
            char updated_address[50];
            strcpy(column_name, "Address");
            printf("Enter updated Address :\n");
            scanf(" %[^\n]", updated_address);
            update_string_prepare2_v3(column_name, updated_address, lawyer_id);
            break;
        case 4:
            char updated_contact[50];
            strcpy(column_name, "Contact_Number");
            printf("Enter updated Contact Number :\n");
            scanf(" %[^\n]", updated_contact);
            update_string_prepare2_v3(column_name, updated_contact, lawyer_id);
            break;
        case 5:
            char updated_qualification[50];
            strcpy(column_name, "Qualification");
            printf("Enter updated Qualification details :\n");
            scanf(" %[^\n]", updated_qualification);
            update_string_prepare2_v3(column_name, updated_qualification, lawyer_id);
            break;
        case 6:
            int updated_experience;
            strcpy(column_name, "Experience");
            printf("Enter updated experience :\n");
            scanf("%d", &updated_experience);
            update_integer_prepare2_v3(column_name, updated_experience, lawyer_id);
            break;
        case 7:
            int updated_com_case;
            strcpy(column_name, "Total_Case");
            printf("Enter updated complete cases :\n");
            scanf("%d", &updated_com_case);
            update_integer_prepare2_v3(column_name, updated_com_case, lawyer_id);
            break;
        case 8:
            char updated_status[50];
            strcpy(column_name, "status");
            printf("Enter updated status :\n");
            scanf(" %[^\n]", updated_status);
            update_string_prepare2_v3(column_name, updated_status, lawyer_id);
            break;
        case 9:
            char updated_court_level[50];
            strcpy(column_name, "court_level");
            printf("Enter updated court level :\n");
            scanf(" %[^\n]", updated_court_level);
            update_string_prepare2_v3(column_name, updated_court_level, lawyer_id);
            break;
        case 10:
            char updated_password[50];
            strcpy(column_name, "Password");
            printf("Enter updated password :\n");
            scanf(" %[^\n]", updated_password);
            update_string_prepare2_v3(column_name, updated_password, lawyer_id);
            break;
        default:
            printf("\nInvalid choice\n");
        }
    }
}
int update_string_prepare3_v3(char column_name[30], char new_name[50], char citizen_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int prepare, rc;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    char update_sql[200];
    sprintf(update_sql, "UPDATE citizen SET %s=? WHERE User_ID=?", column_name);
    prepare = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("\nParses faiing try again later !\n");
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_text(stmt, 1, new_name, -1, NULL);
    sqlite3_bind_text(stmt, 2, citizen_id,-1,NULL);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("\nUpdate failed : %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("\nUpdate successful !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
int update_integer_prepare3_v3(char column_name[30], int new_value, char citizen_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int prepare, rc;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    char update_sql[200];
    sprintf(update_sql, "UPDATE citizen SET %s=? WHERE User_ID=?", column_name);
    prepare = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("\nParses faiing try again later !\n");
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_int(stmt, 1, new_value);
    sqlite3_bind_text(stmt, 2, citizen_id,-1,NULL);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("\nUpdate failed : %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("\nUpdate successful !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
void update_citizen_information(char citizen_id[20])
{
    int col;
    char column_name[30];
    while (1)
    {
        printf("What do you want to update ?\n");
        printf("1)Name\n2)Age\n3)Address\n4)Contact Number\n5)Password\n6)Exit\n");
        scanf("%d", &col);
        if (col == 11)
        {
            break;
        }
        switch (col)
        {
        case 1:
            char updated_name[50];
            strcpy(column_name, "Name");
            printf("Enter updated name :\n");
            scanf(" [%^\n]", updated_name);
            update_string_prepare3_v3(column_name, updated_name, citizen_id);
            break;
        case 2:
            int updated_age;
            strcpy(column_name, "Age");
            printf("Enter updated age :\n");
            scanf("%d", &updated_age);
            update_integer_prepare3_v3(column_name, updated_age,citizen_id);
            break;
        case 3:
            char updated_address[50];
            strcpy(column_name, "Address");
            printf("Enter updated Address :\n");
            scanf(" %[^\n]", updated_address);
            update_string_prepare3_v3(column_name, updated_address, citizen_id);
            break;
        case 4:
            char updated_contact[50];
            strcpy(column_name, "Contact_Number");
            printf("Enter updated Contact Number :\n");
            scanf(" %[^\n]", updated_contact);
            update_string_prepare3_v3(column_name, updated_contact, citizen_id);
            break;
        
        case 5:
            char updated_password[50];
            strcpy(column_name, "Password");
            printf("Enter updated password :\n");
            scanf(" %[^\n]", updated_password);
            update_string_prepare3_v3(column_name, updated_password,citizen_id);
            break;
        default:
            printf("\nInvalid choice\n");
        }
    }
}
int update_string_prepare4_v3(char column_name[30], char new_name[50], char clerk_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int prepare, rc;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    char update_sql[200];
    sprintf(update_sql, "UPDATE clerk SET %s=? WHERE User_ID=?", column_name);
    prepare = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("\nParses faiing try again later !\n");
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_text(stmt, 1, new_name, -1, NULL);
    sqlite3_bind_text(stmt, 2, clerk_id,-1,NULL);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("\nUpdate failed : %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("\nUpdate successful !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
int update_integer_prepare4_v3(char column_name[30], int new_value, char clerk_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int prepare, rc;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    char update_sql[200];
    sprintf(update_sql, "UPDATE clerk SET %s=? WHERE User_ID=?", column_name);
    prepare = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("\nParses faiing try again later !\n");
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_int(stmt, 1, new_value);
    sqlite3_bind_text(stmt, 2, clerk_id,-1,NULL);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("\nUpdate failed : %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("\nUpdate successful !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
int update_string_prepare5_v3(char column_name[30], char new_name[50], char user_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int prepare, rc;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    char update_sql[200];
    sprintf(update_sql, "UPDATE Evidence SET %s=? WHERE User_ID=?", column_name);
    prepare = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("\nParses faiing try again later !\n");
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_text(stmt, 1, new_name, -1, NULL);
    sqlite3_bind_text(stmt, 2, user_id,-1,NULL);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("\nUpdate failed : %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("\nUpdate successful !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
int update_integer_prepare5_v3(char column_name[30], int new_value, char user_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int prepare, rc;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    char update_sql[200];
    sprintf(update_sql, "UPDATE Evidence SET %s=? WHERE User_ID=?", column_name);
    prepare = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("\nParses faiing try again later !\n");
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_int(stmt, 1, new_value);
    sqlite3_bind_text(stmt, 2, user_id,-1,NULL);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("\nUpdate failed : %s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("\nUpdate successful !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
void update_evidence_information(char case_id[20])
{
    int col;
    char column_name[30];
    while (1)
    {
        printf("What do you want to update ?\n");
        printf("1)Criminal Name\n2)Age\n3)Address\n4)Contact Number\n5)Evidence\n6)Exit\n");
        scanf("%d", &col);
        if (col ==6)
        {
            break;
        }
        switch (col)
        {
        case 1:
            char updated_name[50];
            strcpy(column_name, "Criminal_Name");
            printf("Enter updated name :\n");
            scanf(" %[^\n]", updated_name);
            update_string_prepare5_v3(column_name, updated_name, case_id);
            break;
        case 2:
            int updated_age;
            strcpy(column_name, "Age");
            printf("Enter updated age :\n");
            scanf("%d", &updated_age);
            update_integer_prepare5_v3(column_name, updated_age,case_id);
            break;
        case 3:
            char updated_address[50];
            strcpy(column_name, "Address");
            printf("Enter updated Address :\n");
            scanf(" %[^\n]", updated_address);
            update_string_prepare5_v3(column_name, updated_address,case_id);
            break;
        case 4:
            char updated_contact[50];
            strcpy(column_name, "Contact_Number");
            printf("Enter updated Contact Number :\n");
            scanf(" %[^\n]", updated_contact);
            update_string_prepare5_v3(column_name, updated_contact,case_id);
            break;
        
        case 5:
            char updated_evidence[50];
            strcpy(column_name, "Evidence");
            printf("Enter updated password :\n");
            scanf(" %[^\n]", updated_evidence);
            update_string_prepare5_v3(column_name, updated_evidence,case_id);
            break;
        default:
            printf("\nInvalid choice\n");
        }
    }
}
void update_clerk_information(char clerk_id[20])
{
  int col;
    char column_name[30];
    while (1)
    {
        printf("What do you want to update ?\n");
        printf("1)Name\n2)Age\n3)Address\n4)Contact Number\n5)Joined Date\n6)Password\n7)Exit\n");
        scanf("%d", &col);
        if (col == 7)
        {
            break;
        }
        switch (col)
        {
        case 1:
            char updated_name[50];
            strcpy(column_name, "Name");
            printf("Enter updated name :\n");
            scanf(" %[^\n]", updated_name);
            update_string_prepare3_v3(column_name, updated_name, clerk_id);
            break;
        case 2:
            int updated_age;
            strcpy(column_name, "Age");
            printf("Enter updated age :\n");
            scanf("%d", &updated_age);
            update_integer_prepare3_v3(column_name, updated_age,clerk_id);
            break;
        case 3:
            char updated_address[50];
            strcpy(column_name, "Address");
            printf("Enter updated Address :\n");
            scanf(" %[^\n]", updated_address);
            update_string_prepare3_v3(column_name, updated_address,clerk_id);
            break;
        case 4:
            char updated_contact[50];
            strcpy(column_name, "Contact_Number");
            printf("Enter updated Contact Number :\n");
            scanf(" %[^\n]", updated_contact);
            update_string_prepare3_v3(column_name, updated_contact, clerk_id);
            break;
        case 5:
            char updated_joined_date[50];
            strcpy(column_name, "Joined_date");
            printf("Enter updated date :\n");
            scanf(" %[^\n]", updated_joined_date);
            update_string_prepare3_v3(column_name, updated_contact, clerk_id);
            break;
        
        case 6:
            char updated_password[50];
            strcpy(column_name, "Password");
            printf("Enter updated password :\n");
            scanf(" %[^\n]", updated_password);
            update_string_prepare3_v3(column_name, updated_password,clerk_id);
            break;
        default:
            printf("\nInvalid choice\n");
        }
    }
}
int delete_judge_information(char judge_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    const char *sql_delete;
    sql_delete = "DELETE FROM Judge WHERE Judge_ID = ?";
    prepare = sqlite3_prepare_v2(db, sql_delete, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Parsing failed ! : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_text(stmt, 1, judge_id,-1,NULL);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("Deletion failed ! %s\n", sqlite3_errmsg(db));
    }
    else
    {
        int changes = sqlite3_changes(db);
        if (changes == 0)
            printf("No judge found with ID %d.\n", judge_id);
        else
            printf("Deletion Successful! %d record(s) deleted.\n", changes);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
int delete_lawyer_information(char lawyer_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    const char *sql_delete;
    sql_delete = "DELETE FROM Lawyer WHERE Lyr_ID = ?";
    prepare = sqlite3_prepare_v2(db, sql_delete, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Parsing failed ! : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_text(stmt, 1,lawyer_id,-1,NULL);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("Deletion failed ! %s\n", sqlite3_errmsg(db));
    }
    else
    {
        int changes = sqlite3_changes(db);
        if (changes == 0)
            printf("No judge found with ID %d.\n", lawyer_id);
        else
            printf("Deletion Successful! %d record(s) deleted.\n", changes);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
int delete_citizen_information(char citizen_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc, prepare;
    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding !\n");
        return 0;
    }
    const char *sql_delete;
    sql_delete = "DELETE FROM citizen WHERE User_ID = ?";
    prepare = sqlite3_prepare_v2(db, sql_delete, -1, &stmt, NULL);
    if (prepare != SQLITE_OK)
    {
        printf("Parsing failed ! : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }
    sqlite3_bind_text(stmt, 1,citizen_id,-1,NULL);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        printf("Deletion failed ! %s\n", sqlite3_errmsg(db));
    }
    else
    {
        int changes = sqlite3_changes(db);
        if (changes == 0)
            printf("No judge found with ID %d.\n", citizen_id);
        else
            printf("Deletion Successful! %d record(s) deleted.\n", changes);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}

int case_information(char case_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc,found=0;

    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding!\n");
        return 0;
    }

    const char *sql = "SELECT * FROM Crime_table WHERE Case_ID = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("\nParse failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_text(stmt, 1, case_id, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    
    if(rc==SQLITE_ROW)
    {
        found = 1;
        printf("\n=== CASE DETAILS ===\n");
        printf("CASE ID          : %s\n", sqlite3_column_text(stmt, 0));
        printf("FIR NUMBER       : %d\n", sqlite3_column_int(stmt, 1));
        printf("NAME             : %s\n", sqlite3_column_text(stmt, 2));
        printf("JUDGE ID         : %s\n", sqlite3_column_text(stmt, 3));
        printf("FIRST LAWYER ID  : %s\n", sqlite3_column_text(stmt, 4));
        printf("SECOND LAWYER ID : %s\n", sqlite3_column_text(stmt, 5));
        printf("CRIMINAL ID      : %s\n", sqlite3_column_text(stmt, 6));
        printf("CASE HANDLER     : %s\n", sqlite3_column_text(stmt, 7));
        printf("STATUS           : %s\n", sqlite3_column_text(stmt, 8));
        printf("PUNISHMENT       : %s\n", sqlite3_column_text(stmt, 9));
        printf("VERDICT          : %s\n", sqlite3_column_text(stmt, 10));
        printf("CRIME DATE       : %s\n", sqlite3_column_text(stmt, 11));
        printf("INCIDENT PLACE   : %s\n", sqlite3_column_text(stmt, 12));
        printf("COURT LEVEL      : %s\n", sqlite3_column_text(stmt, 13));
    }

    if (!found)
        printf("\nNo case found with ID: %s\n", case_id);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return found;
}
int criminal_information_id(char criminal_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc,found=0;

    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding!\n");
        return 0;
    }

    const char *sql = "SELECT * FROM Criminals WHERE Criminal_ID = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("\nParse failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_text(stmt, 1, criminal_id, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    
    if(rc==SQLITE_ROW)
    {
        found = 1;
        printf("\n   <=== CRIMINAL DETAILS ===>\n");
        printf("CRIMINAL ID          : %s\n", sqlite3_column_text(stmt, 0));
        printf("FIR NUMBER           : %d\n", sqlite3_column_int(stmt, 1));
        printf("NAME                 : %s\n", sqlite3_column_text(stmt, 2));
        printf("AGE                  : %d\n", sqlite3_column_int(stmt, 3));
        printf("Address              : %s\n", sqlite3_column_text(stmt, 4));
        printf("CRIME DATE           : %s\n", sqlite3_column_text(stmt, 5));
        printf("INCIDENT PLACE       : %s\n", sqlite3_column_text(stmt, 6));
        printf("IPC SECTION          : %s\n", sqlite3_column_text(stmt, 7));
        printf("CRIME                : %s\n", sqlite3_column_text(stmt, 8));
        printf("STATUS               : %s\n", sqlite3_column_text(stmt, 9));
        printf("PUNISHMENT           : %s\n", sqlite3_column_text(stmt, 10));
        printf("COURT LEVEL          : %s\n", sqlite3_column_text(stmt, 11));
        printf("CASE HANDLER         : %s\n", sqlite3_column_text(stmt, 12));
    }

    if (!found)
        printf("\nNo case found with ID: %s\n", criminal_id);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return found;
}
int criminal_information_fir(long long int fir_number)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc,found=0;

    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding!\n");
        return 0;
    }

    const char *sql = "SELECT * FROM Criminals WHERE FIR_NUMBER = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("\nParse failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_int(stmt, 1,(int)fir_number);
    rc = sqlite3_step(stmt);
    
    if(rc==SQLITE_ROW)
    {
        found = 1;
        printf("\n   <=== CRIMINAL DETAILS ===>\n");
       // printf("CRIMINAL ID          : %s\n", sqlite3_column_text(stmt, 0));
        printf("FIR NUMBER           : %d\n", sqlite3_column_int(stmt, 1));
        printf("NAME                 : %s\n", sqlite3_column_text(stmt, 2));
        printf("AGE                  : %d\n", sqlite3_column_int(stmt, 3));
        printf("Address              : %s\n", sqlite3_column_text(stmt, 4));
        printf("CRIME DATE           : %s\n", sqlite3_column_text(stmt, 5));
        printf("INCIDENT PLACE       : %s\n", sqlite3_column_text(stmt, 6));
        printf("IPC SECTION          : %s\n", sqlite3_column_text(stmt, 7));
        printf("CRIME                : %s\n", sqlite3_column_text(stmt, 8));
        printf("STATUS               : %s\n", sqlite3_column_text(stmt, 9));
        printf("PUNISHMENT           : %s\n", sqlite3_column_text(stmt, 10));
        printf("COURT LEVEL          : %s\n", sqlite3_column_text(stmt, 11));
        printf("CASE HANDLER         : %s\n", sqlite3_column_text(stmt, 12));
    }

    if (!found)
        printf("\nNo case found with ID: %s\n", fir_number);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return found;
}
void evidence_information(char case_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc,found=0,pass=0;

    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding!\n");
        return ;
    }

    const char *sql = "SELECT * FROM Evidence WHERE Case_ID = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("\nParse failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }

    sqlite3_bind_text(stmt, 1, case_id, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    
    if(rc==SQLITE_ROW)
    {
        found = 1;
        printf("\n    <=== USER DETAILS ===> \n");
        printf("CASE ID            : %s\n",sqlite3_column_text(stmt, 0));
        printf("CRIMINAL NAME      : %s\n", sqlite3_column_text(stmt, 1));
        printf("AGE                : %d\n",sqlite3_column_int(stmt, 2));
        printf("EVIDENCE           : %s\n", sqlite3_column_text(stmt, 3));
        printf("ADDRESS            : %s\n", sqlite3_column_text(stmt, 4));
        printf("CONTACT NUMBER     : %s\n",sqlite3_column_text(stmt, 5));
       
    }

    if (!found)
        printf("\nNo case found with ID: %s\n", case_id);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
void Judge_Profile(char JUDGE_ID[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc,found=0,pass=0;

    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding!\n");
        return ;
    }

    const char *sql = "SELECT * FROM Judge WHERE Judge_ID = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("\nParse failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }

    sqlite3_bind_text(stmt, 1,JUDGE_ID, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    
    if(rc==SQLITE_ROW)
    {
        found = 1;
        printf("\n    <=== JUDGE DETAILS ===> \n");
        printf("JUDGE ID          : %s\n",sqlite3_column_text(stmt, 0));
        printf("NAME              : %s\n", sqlite3_column_text(stmt, 1));
        printf("AGE               : %d\n",sqlite3_column_int(stmt, 2));
        printf("ADDRESS           : %s\n", sqlite3_column_text(stmt, 3));
        printf("CONTACT NUMBERR   : %s\n",sqlite3_column_text(stmt, 4));
        printf("QUALIFICATION     : %s\n", sqlite3_column_text(stmt, 5));
        printf("EXPERIENCE        : %d\n",sqlite3_column_int(stmt, 6));
        printf("COMPLETE CASES    : %d\n", sqlite3_column_int(stmt, 7));
        printf("STATUS            : %s\n", sqlite3_column_text(stmt, 8));
        printf("COURT LEVEL       : %s\n", sqlite3_column_text(stmt, 9));
        printf("PASSWORD          : **********\n");
        printf("SHOW PASSWORD (PRESS 1 IF YES OR PRESS 0 FOR NO):\n");
        scanf("%d",&pass);
        if(pass)
        {
        printf("PASSWORD           : %s\n",sqlite3_column_text(stmt, 10));
        }
    }

    if (!found)
        printf("\nNo profile found with ID: %s\n", JUDGE_ID);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
void Lawyer_Profile(char LAWYER_ID[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc,found=0,pass=0;

    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding!\n");
        return ;
    }

    const char *sql = "SELECT * FROM Lawyer WHERE Lyr_ID = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("\nParse failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }

    sqlite3_bind_text(stmt, 1,LAWYER_ID, -1, NULL);
    rc = sqlite3_step(stmt);
    
    if(rc==SQLITE_ROW)
    {
        found = 1;
        printf("\n    <=== LAWYER DETAILS ===> \n");
        printf("LAWYER ID          : %s\n",sqlite3_column_text(stmt, 0));
        printf("NAME               : %s\n", sqlite3_column_text(stmt, 1));
        printf("AGE                : %d\n",sqlite3_column_int(stmt, 2));
        printf("ADDRESS            : %s\n", sqlite3_column_text(stmt, 3));
        printf("CONTACT NUMBERR    : %s\n",sqlite3_column_text(stmt, 4));
        printf("QUALIFICATION      : %s\n", sqlite3_column_text(stmt, 5));
        printf("EXPERIENCE         : %d\n",sqlite3_column_int(stmt, 6));
        printf("TOTAL CASE         : %d\n", sqlite3_column_int(stmt, 7));
        printf("WON                : %d\n", sqlite3_column_int(stmt, 8));
        printf("STATUS             : %s\n", sqlite3_column_text(stmt, 9));
        printf("COURT LEVEL        : %s\n", sqlite3_column_text(stmt, 10));
        printf("PASSWORD           : **********\n");
        printf("SHOW PASSWORD (PRESS 1 IF YES OR PRESS 0 FOR NO):\n");
        scanf("%d",&pass);
        if(pass)
        {
        printf("PASSWORD           : %s\n",sqlite3_column_text(stmt, 11));
        }
    }

    if (!found)
        printf("\nNo profile found with ID: %s\n", LAWYER_ID);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
void Citizen_Profile(char citizen_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc,found=0,pass=0;

    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding!\n");
        return ;
    }

    const char *sql = "SELECT * FROM citizen WHERE User_ID = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("\nParse failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }

    sqlite3_bind_text(stmt, 1,citizen_id, -1, NULL);
    rc = sqlite3_step(stmt);
    
    if(rc==SQLITE_ROW)
    {
        found = 1;
        printf("\n    <=== USER DETAILS ===> \n");
        printf("USER ID            : %s\n",sqlite3_column_text(stmt, 0));
        printf("NAME               : %s\n", sqlite3_column_text(stmt, 1));
        printf("AGE                : %d\n",sqlite3_column_int(stmt, 2));
        printf("ADDRESS            : %s\n", sqlite3_column_text(stmt, 3));
        printf("CONTACT NUMBERR    : %s\n",sqlite3_column_text(stmt, 4));
        printf("PASSWORD           : **********\n");
        printf("SHOW PASSWORD (PRESS 1 IF YES OR PRESS 0 FOR NO):\n");
        scanf("%d",&pass);
        if(pass)
        {
        printf("PASSWORD           : %s\n",sqlite3_column_text(stmt, 5));
        }
    }

    if (!found)
    {
        printf("\nNo profile found with ID: %s\n",citizen_id);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);  
}
void Clerk_Profile(char clerk_id[20])
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc,found=0,pass=0;

    rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("\nServer not responding!\n");
        return ;
    }

    const char *sql = "SELECT * FROM clerk WHERE User_ID = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("\nParse failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }

    sqlite3_bind_text(stmt, 1,clerk_id, -1, NULL);
    rc = sqlite3_step(stmt);
    
    if(rc==SQLITE_ROW)
    {
        found = 1;
        printf("\n    <=== USER DETAILS ===> \n");
        printf("USER ID            : %s\n",sqlite3_column_text(stmt, 0));
        printf("NAME               : %s\n", sqlite3_column_text(stmt, 1));
        printf("AGE                : %d\n",sqlite3_column_int(stmt, 2));
        printf("ADDRESS            : %s\n", sqlite3_column_text(stmt, 3));
        printf("CONTACT NUMBER     : %s\n",sqlite3_column_text(stmt, 4));
        printf("JOINED DATE        : %s\n",sqlite3_column_text(stmt, 5));
        printf("PASSWORD           : **********\n");
        printf("SHOW PASSWORD (PRESS 1 IF YES OR PRESS 0 FOR NO):\n");
        scanf("%d",&pass);
        if(pass)
        {
        printf("PASSWORD           : %s\n",sqlite3_column_text(stmt, 6));
        }
    }

    if (!found)
    {
      printf("\nNo profile found with ID: %s\n",clerk_id);
    }
        

    sqlite3_finalize(stmt);
    sqlite3_close(db);  
}
void judge_dashboard(char JUDGE_ID[20])
{
    int n, m;
    char CASE_ID[20];
    while(1)
    {
    printf("-|-----------------------|-\n");
    printf(" | 1) Profile            |\n");
    printf(" | 2) Update Profile     |\n");
    printf(" | 3) Case Information   |\n");
    printf(" | 4) Logout             |\n");
    printf("-|-----------------------|-\n");
    printf("Enter who are you :");
    scanf("%d", &n);
    if(n==4)
    {
        break;
    }
    switch(n)
    {
        case 1:
            Judge_Profile(JUDGE_ID);
        break;
        case 2:
        update_judge_information(JUDGE_ID);
        break;
        case 3:
        printf("\nEnter CASE ID :\n");
        scanf(" %[^\n]",CASE_ID);
        case_information(CASE_ID);
        break;
        default :
        printf("\nInvalid choice\n");

    }
}
}
void lawyer_dashboard(char LAWYER_ID[20])
{
    int n, m;
    char criminal_id[20];
    while(1)
    {
    printf("-|--------------------------|-\n");
    printf(" | 1) Profile               |\n");
    printf(" | 2) Update Profile        |\n");
    printf(" | 3) Criminal Information  |\n");
    printf(" | 4) Logout                |\n");
    printf("-|--------------------------|-\n");
    printf("Enter who are you :");
    scanf("%d", &n);
    if(n==4)
    {
        break;
    }
    switch(n)
    {
        case 1:
        Lawyer_Profile(LAWYER_ID);
        break;
        case 2:
        update_lawyer_information(LAWYER_ID);
        break;
        case 3:
        printf("\nEnter CASE ID :\n");
        scanf(" %[^\n]",criminal_id);
        criminal_information_id(criminal_id);
        break;
        default :
        printf("\nInvalid choice\n");

    }
}
}
void citizen_dashboard(char CITIZEN_ID[20])
{
    int n, m;
    long long int fir_number;
    while(1)
    {
    printf("-|--------------------------|-\n");
    printf(" | 1) Profile               |\n");
    printf(" | 2) Update Profile        |\n");
    printf(" | 3) Criminal Information  |\n");
    printf(" | 4) Logout                |\n");
    printf("-|--------------------------|-\n");
    printf("Enter who are you :");
    scanf("%d", &n);
    if(n==4)
    {
        break;
    }
    switch(n)
    {
        case 1:
        Citizen_Profile(CITIZEN_ID);
        break;
        case 2:
        update_citizen_information(CITIZEN_ID);
        break;
        case 3:
        printf("\nEnter FIR Number :\n");
        scanf("%lld",&fir_number);
        criminal_information_fir(fir_number);
        break;
        default :
        printf("\nInvalid choice\n");

    }
}
}
void clerk_dashboard(char clerk_id[20])
{
    int n, m, age;
    char judge_id[20], lawyer_id[20], citizen_id[20], case_id[20], criminal_id[20];
    char criminal_name[20], evidence[200], contact_number[16];
    long long int Fir_number;
    
    // Combined declarations - duplicates removed
    char name[30], address[50], crime_date[13], Incident_place[50], IPC_section[20], crime[50], status[20], punishment[40], court_level[20], police_name[20];
    char lyr1_id[20], lyr2_id[20], case_handler[30], verdict[13];

    // I used crime_date[13] (the larger of the two) to be safe.
    
    while(1)
    {
    //...
    printf("-|---------------------------------|-\n");
    printf(" |    <====  DASHBOARD ====>       |\n");
    printf("-|---------------------------------|-\n");
    printf(" | 1) Profile                      |\n");
    printf(" | 2) Update Profile               |\n");
    printf(" | 3) Crime Table Entry            |\n");
    printf(" | 4) Criminal Table Entry         |\n");
    printf(" | 5) Evidence Table Entry         |\n");
    printf(" | 6) Judge Information            |\n");
    printf(" | 7) Update Judge Information     |\n");
    printf(" | 8) Delete Judge Information     |\n");
    printf(" | 9) Lawyer Information           |\n"); 
    printf(" | 10) Update Lawyer Information   |\n"); 
    printf(" | 11) Delete Lawyer Information   |\n"); 
    printf(" | 12) Citizen Information         |\n");
    printf(" | 13) Update citizen Information  |\n"); 
    printf(" | 14) Delete citizen Information  |\n");
    printf(" | 15) Criminal Information        |\n");
    printf(" | 16) Any Case Information        |\n");  
    printf(" | 17) Evidence Information        |\n");
    printf(" | 18) Update Evidence Information |\n");
    printf(" | 19) Logout                      |\n");
    printf("-|---------------------------------|-\n");
    printf("\nEnter \n :");
    scanf("%d", &n);
    if(n==19)
    {
        break;
    }
    switch(n)
    {
        case 1:
        Clerk_Profile(clerk_id);
        break;
        case 2:
        update_clerk_information(clerk_id);
        break;
        case 3:
        printf("Enter Criminal Name : \n");
    scanf(" %[^\n]", name);
    printf("Enter Incident place :\n");
    scanf(" %[^\n]", Incident_place);
    printf("Enter case holder name :\n");
    scanf(" %[^\n]", case_handler);
    printf("Enter punishment details :\n");
    scanf(" %[^\n]", punishment);
    printf("Enter crime date  :\n");
    scanf(" %[^\n]", crime_date);
    printf("Enter status of case :\n");
    scanf(" %[^\n]", status);
    printf("Enter level of court :\n");
    scanf(" %[^\n]", court_level);
    printf("Enter verdict :\n");
    scanf(" %[^\n]", verdict);
    printf("Enter Judge Id :\n");
    scanf(" %[^\n]", judge_id);
    printf("Enter case Id : \n");
    scanf(" %[^\n]", case_id);
    printf("Enter lawyer 1 Id :");
    scanf(" %[^\n]", lyr1_id);
    printf("Enter lawyer 2 Id :");
    scanf(" %[^\n]", lyr2_id);
    printf("Enter criminal Id :");
    scanf(" %[^\n]", criminal_id);
    printf("Enter FIR Number :");
    scanf("%lld", &Fir_number);
        crime_table_entry(case_id,Fir_number,name,judge_id,lyr1_id,lyr2_id,criminal_id,case_handler,verdict,punishment,status,crime_date,Incident_place,court_level);
        break;
        case 4:
        printf("\nEnter criminal id :\n");
    scanf(" %[^\n]", criminal_id);
    printf("Enter name : \n");
    scanf(" %[^\n]", name);
    printf("Enter crime date :\n");
    scanf(" %[^\n]", crime_date);
    printf("Enter  Address :\n");
    scanf(" %[^\n]", address);
    printf("Enter Incident place :\n");
    scanf(" %[^\n]", Incident_place);
    printf("Enter IPC Section :\n");
    scanf(" %[^\n]", IPC_section);
    printf("Enter crime details :\n");
    scanf(" %[^\n]", crime);
    printf("Enter punishment details :\n");
    scanf(" %[^\n]", punishment);
    printf("Enter status of case :\n");
    scanf(" %[^\n]", status);
    printf("Enter court level :\n");
    scanf(" %[^\n]", court_level);
    printf("Enter the case holder (police name) :");
    scanf(" %[^\n]", police_name);
    printf("Enter FIR NUMBER :\n");
    scanf("%lld",&Fir_number);
    printf("Enter AGE  :\n");
    scanf("%d",&age);
        criminals_entry(criminal_id,Fir_number,name,age,address,crime_date,Incident_place,IPC_section,crime,status,punishment,court_level,police_name);
        break;
        case 5:
    printf("Enter case Id : \n");
    scanf(" %[^\n]", case_id);
    printf("Enter Criminal Name : \n");
    scanf(" %[^\n]", criminal_name);
    printf("Enter Evidence :\n");
    scanf(" %[^\n]", evidence);
    printf("Enter Address :\n");
    scanf(" %[^\n]", address);
    printf("Enter contact number \n");
    scanf(" %[^\n]", contact_number);
    printf("Enter Age :");
    scanf("%d", &age);
        evidence_entry(case_id,criminal_name,age,evidence,address,contact_number);
        break;
        case 6:
        printf("\nEnter judge id :\n");
        scanf(" %[^\n]",judge_id);
        Judge_Profile(judge_id);
        break;
        case 7:
        printf("\nEnter judge id :\n");
        scanf(" %[^\n]",judge_id);        
        update_judge_information(judge_id);
        break;
        case 8:
        printf("\nEnter judge id :\n");
        scanf(" %[^\n]",judge_id);
        delete_judge_information(judge_id);
        break;
        case 9:
        printf("\nEnter Lawyer ID :\n");
        scanf(" %[^\n]",lawyer_id);
        Lawyer_Profile(lawyer_id);
        break;
        case 10:
        printf("\nEnter Lawyer ID :\n");
        scanf(" %[^\n]",lawyer_id);
        update_lawyer_information(lawyer_id);
        break;
        case 11:
        printf("\nEnter Lawyer ID :\n");
        scanf(" %[^\n]",lawyer_id);
        delete_lawyer_information(lawyer_id);
        break;
        case 12:
        printf("\nEnter Citizen ID :\n");
        scanf(" %[^\n]",citizen_id);
        Citizen_Profile(citizen_id);
        break;
        case 13:
        printf("\nEnter Citizen ID :\n");
        scanf(" %[^\n]",citizen_id);
        update_citizen_information(citizen_id);
        break;
        case 14:
        printf("\nEnter Citizen ID :\n");
        scanf(" %[^\n]",citizen_id);
        delete_citizen_information(citizen_id);
        break; 
        case 15:
        printf("\nEnter Criminal ID :\n");
        scanf(" %[^\n]",criminal_id);
        criminal_information_id(criminal_id);
        break;
        case 16:
        printf("\nEnter Case ID :\n");
        scanf(" %[^\n]",case_id);
        case_information(case_id);
        break;
        case 17:
        printf("\nEnter case id :\n");
        scanf(" %[^\n]",case_id);
        evidence_information(case_id);
        break;
        case 18:
         printf("\nEnter case id :\n");
        scanf(" %[^\n]",case_id);
        update_evidence_information(case_id);
        break;
        default :
        printf("\nInvalid choice\n");

    }
   }

}
void lead_author()
{
    int n, m,age,confirm;
     char USER_ID[20],name[20],Address[50],contact_number[13],password[20],user_id[20],joined_date[15];
    while(1)
    {
    printf("-|--------------------|-\n");
    printf(" | 1) Judge           |\n");
    printf(" | 2) Lawyer          |\n");
    printf(" | 3) Citizen         |\n");
    printf(" | 4) Clerk           |\n");
    printf(" | 5) About           |\n");
    printf(" | 6) Exit            |\n");
    printf("-|--------------------|-\n");
    printf("Enter who are you :");
    scanf("%d", &n);
    if(n==6)
    {
        break;
    }
    switch (n)
    {
    case 1:   //Judge domain
        printf("Already have a account? (if yes press 1 no press 0)\n");
        scanf("%d", &m);
        if (m)
        {

            printf("Enter ID :");
            scanf(" %[^\n]",USER_ID);
            if(Judge_login(USER_ID))
            {
                judge_dashboard(USER_ID);
            }
        }                                  
        else
        {   
            Judge_registration();
             printf("\n");
            printf("Enter ID :");
            scanf(" %[^\n]",USER_ID); 
            if(Judge_login(USER_ID))
            {
                judge_dashboard(USER_ID);
            }
          
        }
        break;
    case 2:  // Lawyer domain
        printf("Already have a account? (if yes press 1 no press 0)\n");
        scanf("%d", &m);
        if (m)
        {
            printf("Enter ID :");
            scanf(" %[^\n]",USER_ID);
            if(Lawyer_login(USER_ID))
            {
                lawyer_dashboard(USER_ID);
            }
        }                                                     
        else
        {
            lawyer_registration();
             printf("\n");
            printf("Enter ID :");
            scanf(" %[^\n]",USER_ID);
            if(Lawyer_login(USER_ID))
            {
                lawyer_dashboard(USER_ID);
            }
        }
        break;

    case 3:    //citizen domain
        printf("Already have a account? (if yes press 1 no press 0)\n");
        scanf("%d", &m);
        if (m)
        {
            printf("Enter ID :");
            scanf(" %[^\n]",USER_ID);
            if(Citizen_login(USER_ID))
            {
                citizen_dashboard(USER_ID);
            }
        }
        else
        {
            citizen_registration();
            printf("\n");
            printf("Enter ID :");
            scanf(" %[^\n]",USER_ID);
            if(Citizen_login(USER_ID))
            {
                citizen_dashboard(USER_ID);
            }
        }
        break;
    case 4:     //Clerk domain
        printf("Already have a account? (if yes press 1 no press 0)\n");
        scanf("%d", &m);
        if (m)
        {
            printf("Enter ID :");
            scanf(" %[^\n]",USER_ID);
            if(clerk_login(USER_ID))
            {
                clerk_dashboard(USER_ID);
            }
        }
        else
        {
            printf("Enter Id :\n");
    scanf(" %[^\n]", user_id);
    printf("Enter name : \n");
    scanf(" %[^\n]", name);
    printf("Enter contact number :\n");
    scanf(" %[^\n]", contact_number);
    printf("Enter  Address :\n");
    scanf(" %[^\n]", Address);
    printf("Enter Joined Date :\n");
    scanf(" %[^\n]", joined_date);
    printf("Kindly enter password :\n");
    scanf(" %[^\n]", password);
    printf("Enter Age : \n");
    scanf("%d", &age);
            clerk_registration(age,name,Address,contact_number,password,user_id,joined_date);
             printf("\n");
            printf("Enter ID :");
            scanf(" %[^\n]",USER_ID);
            if(clerk_login(USER_ID))
            {
                clerk_dashboard(USER_ID);
            }
        }
          break;  
    case 5:
        printf("We are students and working on to improve court management of dealing with criminals details and provide one  place to gets their details  and manage new one!\n");
        break;
    default:
        printf("This not in our project related choice!");
    }
}
}
int main()
{ 

    creating_database();
    lead_author();
    return 0;
}