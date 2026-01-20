#ifndef JUDECIOMENT_H
#define JUDECIOMENT_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sqlite3.h>
#include<gtk/gtk.h>
static inline void *creating_database(void* arg)
{
    int *return_int = (int *)arg;
    sqlite3 *db;
    char *errmsg;
    int open,create1,create2,create3,create4,create5,create6;
    open=sqlite3_open("Judgment.db",&db);
    if(open!=SQLITE_OK)
    {
        printf("File not opened!\n");
        return_int = (int *)g_memdup2(&(int){0},sizeof(int));
        return NULL;
    }
    sqlite3_exec(db,"PRAGMA foreign keys = ON",NULL,NULL,NULL);

    // JUDGE Table (No Change Requested)
    const char *create_table1 = "CREATE TABLE IF NOT EXISTS Judge("
    "Judge_ID INTEGER PRIMARY KEY ,"
    "Name TEXT NOT NULL,"
    "Age INT,"
    "Address TEXT NOT NULL,"
    "Contact_Number TEXT ,"
    "Qualification TEXT NOT NULL,"
    "Experience INT NOT NULL,"
    "Complete_cases TEXT ,"
    "status TEXT,"
    "court_level INT,"
    "Password INT NOT NULL);";
    create1=sqlite3_exec(db,create_table1,NULL,NULL,NULL);
    if(create1!=SQLITE_OK)
    {
        fprintf(stderr," JUDGE table not created ! : %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return_int = (int *)g_memdup2(&(int){0},sizeof(int));
        return NULL;
    }
    else
    {
        printf(" JUDGE table created\n");
    }

    // LAWYER Table (No Change Requested)
    const char *create_table2="CREATE TABLE IF NOT EXISTS Lawyer("
    "Lyr_ID INTEGER PRIMARY KEY,"
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
    "Password INT NOT NULL);";
    create2=sqlite3_exec(db,create_table2,NULL,NULL,&errmsg);
    if(create2!=SQLITE_OK)
    {
        fprintf(stderr,"LAWYER table is not created : %s\n",errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return_int = (int *)g_memdup2(&(int){0},sizeof(int));
        return NULL;
    }
    else
    {
        printf("\n LAWYER table is created \n");
    }

    // CRIMINALS Table (Matches provided schema)
    const char *create_table3 = "CREATE TABLE IF NOT EXISTS Criminals("
    "Criminal_ID INT PRIMARY KEY,"
    "Name TEXT NOT NULL,"
    "Age INT,"
    "Address TEXT,"
    "Crime_date DATE DEFAULT CURRENT_DATE,"
    "Incident_Place TEXT NOT NULL,"
    "IPC_Section INT NOT NULL,"
    "Crime TEXT NOT NULL,"
    "Punishment TEXT NOT NULL,"
    "Police_holder_name TEXT);";
    create3=sqlite3_exec(db,create_table3,NULL,NULL,NULL);
    if(create3!=SQLITE_OK)
    {
        fprintf(stderr,"CRIMINALS table not created : %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return_int = (int *)g_memdup2(&(int){0},sizeof(int));
        return NULL;
    }
    else
    {
        printf("CRIMINALS table created !\n");
    }

    // CRIME_TABLE (Added IF NOT EXISTS to match other tables)
    const char *create_table4 = "CREATE TABLE IF NOT EXISTS Crime_table ("
    "Case_ID    INTEGER,"
    "FIR_Number INTEGER,"
    "Name   TEXT NOT NULL,"
    "Judge_ID   INTEGER,"
    "Lyr1_ID    INTEGER,"
    "Lyr2_ID    INTEGER,"
    "Criminal_ID    INTEGER,"
    "Police_handler Text NOT NULL,"
    "status TEXT,"
    "Punishment TEXT,"
    "Verdict    TEXT,"
    "Crime_date DATE DEFAULT CURRENT_DATE,"
    "Incident_Place TEXT NOT NULL,"
    "court_level    TEXT,"
    "PRIMARY KEY(FIR_Number),"
    "FOREIGN KEY(Criminal_ID) REFERENCES Criminals(Criminal_ID),"
    "FOREIGN KEY(Judge_ID) REFERENCES Judge(Judge_ID),"
    "FOREIGN KEY(Lyr1_ID) REFERENCES Lawyer(Lyr_ID),"
    "FOREIGN KEY(Lyr2_ID) REFERENCES Lawyer(Lyr_ID));";

    create4=sqlite3_exec(db,create_table4,NULL,NULL,NULL);
    if(create4!=SQLITE_OK)
    {
        fprintf(stderr,"\nCRIME table is not created : %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
       return_int = (int *)g_memdup2(&(int){0},sizeof(int));
        return NULL;
    }
    else
    {
        printf("\nCRIME table created !\n");
    }

    // OTHER Table (No Change Requested)
    const char *create_table5="CREATE TABLE IF NOT EXISTS Other("
    "User_ID INT PRIMARY KEY,"
    "Name TEXT NOT NULL,"
    "Age INT NOT NULL,"
    "Contact_Number TEXT,"
    "Password INT NOT NULL);";  
    create5=sqlite3_exec(db,create_table5,NULL,NULL,NULL);
    if(create5!=SQLITE_OK)
    {
        fprintf(stderr,"OTHER table is not created : %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return_int = (int *)g_memdup2(&(int){0},sizeof(int));
        return NULL;
    }
    else
    {
        printf("OTHER table created !\n");
    }

    // CASE_SCHEDULE Table (Updated to match provided schema)
    const char *create_table6 = "CREATE TABLE IF NOT EXISTS case_schedule ("
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
    
    create6=sqlite3_exec(db,create_table6,NULL,NULL,NULL);
    if(create6!=SQLITE_OK)
    {
        fprintf(stderr,"case_schedule table is not created : %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return_int = (int *)g_memdup2(&(int){0},sizeof(int));
        return NULL;
    }
    else
    {
        printf("case_schedule table created !\n");
    }
    
    sqlite3_close(db);
    *return_int = *(int *)g_memdup2(&(int){1},sizeof(int));
    return NULL;
}
static inline int Judge_login(const char *userid, const char *pass)
{
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open DB: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    /* parameterized query — faster and safer */
    const char *sql = "SELECT Password FROM Judge WHERE Judge_ID = ?1";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "prepare failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    int user_id = atoi(userid);
    int passwd = atoi(pass);

    sqlite3_bind_int(stmt, 1, user_id);

    int found = 0;
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int password = sqlite3_column_int(stmt, 0); /* column 0 is Password */
        if (password == passwd) {
            found = 1;
            printf("\nLogin successful!\n");
        }
    } else if (rc == SQLITE_DONE) {
        /* no row */
    } else {
        fprintf(stderr, "step error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return found;
}

static inline int Lawyer_login(const char *userid, const char *pass)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc,prepare;
    rc=sqlite3_open("Judgment.db",&db);
    if(rc!=SQLITE_OK)
    {
        printf("Database not responding !\n");
        return 0;
    }
    const char *retrieve_data="SELECT Lyr_ID,Password from Lawyer";
    prepare=sqlite3_prepare_v2(db,retrieve_data,-1,&stmt,NULL);
    if(prepare!=SQLITE_OK)
    {
        printf("Server problem !\n");
        return 0;
    }

   int user_id,passwd,data,found=0;
   user_id = atoi(userid);
   passwd = atoi(pass);
    while((data=sqlite3_step(stmt))==SQLITE_ROW)
    {
        int lawyer_id=sqlite3_column_int(stmt,0);
        int password=sqlite3_column_int(stmt,1);
        if(lawyer_id==user_id && password==passwd)
        {
          printf("\nLogin successfull !\n");
          found=1;
          return 1;
        }
    }
    if(!found)
    {
      printf("Either User_id , password invalid or may this account not exist !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

static inline int Citizen_login(const char *userid, const char *pass)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc,prepare;
    rc=sqlite3_open("Judgment.db",&db);
    if(rc!=SQLITE_OK)
    {
        printf("Database not responding !\n");
        return 0;
    }
    const char *retrieve_data="SELECT User_ID,Password from Other";
    prepare=sqlite3_prepare_v2(db,retrieve_data,-1,&stmt,NULL);
    if(prepare!=SQLITE_OK)
    {
        printf("Server problem !\n");
        return 0;
    }

   int user_id,passwd,data,found=0;
   user_id = atoi(userid);
   passwd = atoi(pass);
    while((data=sqlite3_step(stmt))==SQLITE_ROW)
    {
        int User_id=sqlite3_column_int(stmt,0);
        int password=sqlite3_column_int(stmt,4);
        if(User_id==user_id && password==passwd)
        {
          printf("\nLogin successfull !\n");
          found=1;
          return 1;
        }
    }
    if(!found)
    {
      printf("Either User_id , password invalid or may this account not exist !\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}
#endif