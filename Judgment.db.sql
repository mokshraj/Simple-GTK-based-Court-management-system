BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "Categories" (
	"key"	TEXT,
	"name"	TEXT NOT NULL DEFAULT 'Civil',
	PRIMARY KEY("key")
);
CREATE TABLE IF NOT EXISTS "Crime_table" (
	"Case_ID"	INTEGER,
	"FIR_Number"	INTEGER,
	"Name"	TEXT NOT NULL,
	"Judge_ID"	INTEGER,
	"Lyr1_ID"	INTEGER,
	"Lyr2_ID"	INTEGER,
	"Criminal_ID"	INTEGER,
	"Police_handler"	Text NOT NULL,
	"status"	TEXT,
	"Punishment"	TEXT,
	"Verdict"	TEXT,
	"Crime_date"	DATE DEFAULT CURRENT_DATE,
	"Incident_Place"	TEXT NOT NULL,
	"court_level"	TEXT,
	"Total_Hearings"	INTEGER NOT NULL DEFAULT 0,
	"type"	TEXT NOT NULL DEFAULT 'CI',
	PRIMARY KEY("FIR_Number"),
	FOREIGN KEY("Criminal_ID") REFERENCES "Criminals"("Criminal_ID"),
	FOREIGN KEY("Judge_ID") REFERENCES "Judge"("Judge_ID"),
	FOREIGN KEY("Lyr1_ID") REFERENCES "Lawyer"("Lyr_ID"),
	FOREIGN KEY("Lyr2_ID") REFERENCES "Lawyer"("Lyr_ID")
);
CREATE TABLE IF NOT EXISTS "Criminals" (
	"Criminal_ID"	INT,
	"Name"	TEXT NOT NULL,
	"Age"	INT,
	"Address"	TEXT,
	"Crime_date"	DATE DEFAULT CURRENT_DATE,
	"Incident_Place"	TEXT NOT NULL,
	"IPC_Section"	INT NOT NULL,
	"Crime"	TEXT NOT NULL,
	"Punishment"	TEXT NOT NULL,
	"Police_holder_name"	TEXT,
	PRIMARY KEY("Criminal_ID")
);
CREATE TABLE IF NOT EXISTS "Evidence" (
	"Case_ID"	TEXT,
	"Criminal_Name"	TEXT NOT NULL,
	"Age"	INT NOT NULL,
	"Evidence"	TEXT NOT NULL,
	"Address"	TEXT NOT NULL,
	"Contact_Number"	TEXT,
	PRIMARY KEY("Case_ID")
);
CREATE TABLE IF NOT EXISTS "Judge" (
	"Judge_ID"	INTEGER,
	"Name"	TEXT NOT NULL,
	"Age"	INT,
	"Address"	TEXT NOT NULL,
	"Contact_Number"	TEXT,
	"Qualification"	TEXT NOT NULL,
	"Experience"	INT NOT NULL,
	"Complete_cases"	TEXT,
	"status"	TEXT,
	"court_level"	INT,
	"Password"	INT NOT NULL,
	"type"	TEXT NOT NULL DEFAULT 'CI',
	PRIMARY KEY("Judge_ID")
);
CREATE TABLE IF NOT EXISTS "Lawyer" (
	"Lyr_ID"	INTEGER,
	"Name"	TEXT NOT NULL,
	"Age"	INT,
	"Address"	TEXT NOT NULL,
	"Contact_Number"	TEXT,
	"Qualification"	TEXT NOT NULL,
	"Experience"	INT NOT NULL,
	"Total_Case"	INT NOT NULL,
	"Won"	INT,
	"status"	TEXT,
	"court_level"	TEXT,
	"Password"	INT NOT NULL,
	"type"	TEXT NOT NULL DEFAULT 'CI',
	PRIMARY KEY("Lyr_ID")
);
CREATE TABLE IF NOT EXISTS "Other" (
	"User_ID"	INT,
	"Name"	TEXT NOT NULL,
	"Age"	INT NOT NULL,
	"Contact_Number"	TEXT,
	"Password"	INT NOT NULL,
	PRIMARY KEY("User_ID")
);
CREATE TABLE IF NOT EXISTS "case_schedule" (
	"schedule_id"	INTEGER,
	"Case_id"	INTEGER NOT NULL,
	"clerk_id"	INTEGER,
	"courtroom_no"	TEXT,
	"date"	DATE NOT NULL,
	"start_time"	TIME NOT NULL,
	"end_time"	TIME,
	"status"	TEXT DEFAULT 'Scheduled',
	"remarks"	TEXT,
	"created_at"	DATETIME DEFAULT CURRENT_TIMESTAMP,
	"updated_at"	DATETIME DEFAULT CURRENT_TIMESTAMP,
	"Video_id"	TEXT,
	"Proceding_time"	TEXT,
	"Break_time"	TEXT,
	PRIMARY KEY("schedule_id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "citizen" (
	"User_ID"	TEXT,
	"Name"	TEXT NOT NULL,
	"Age"	INT NOT NULL,
	"Address"	TEXT NOT NULL,
	"Contact_Number"	TEXT,
	"Password"	TEXT NOT NULL,
	PRIMARY KEY("User_ID")
);
CREATE TABLE IF NOT EXISTS "clerk" (
	"User_ID"	TEXT,
	"Name"	TEXT NOT NULL,
	"Age"	INT NOT NULL,
	"Address"	TEXT NOT NULL,
	"Contact_Number"	TEXT,
	"Joined_date"	DATE DEFAULT CURRENT_DATE,
	"Password"	TEXT NOT NULL,
	PRIMARY KEY("User_ID")
);
INSERT INTO "Categories" VALUES ('CI','Civil');
INSERT INTO "Categories" VALUES ('CR','Criminal');
INSERT INTO "Categories" VALUES ('FM','Family');
INSERT INTO "Categories" VALUES ('CO','Corporate');
INSERT INTO "Categories" VALUES ('TX','Tax');
INSERT INTO "Crime_table" VALUES (1,1,'Meerut murder case (March 2025)',110,101,102,1,'',NULL,NULL,NULL,'2025-11-14','',NULL,1,'civil');
INSERT INTO "Crime_table" VALUES (2,2,'Atul Subhash case',110,101,102,NULL,'',NULL,NULL,NULL,'2025-11-14','',NULL,0,'civil');
INSERT INTO "Crime_table" VALUES (3,3,'The man who sued himself (Manoj Kumar)',110,101,102,NULL,'',NULL,NULL,NULL,'2025-11-14','',NULL,0,'civil');
INSERT INTO "Crime_table" VALUES (4,4,'Burari Case',101,101,102,NULL,'',NULL,NULL,NULL,'2025-11-14','',NULL,2,'Family');
INSERT INTO "Criminals" VALUES (1,'',NULL,NULL,'2025-11-14','',0,'','',NULL);
INSERT INTO "Judge" VALUES (101,'Prashant',60,'Transport Nagar','695874312','BCA',12,'100','yes','Supreme court',101,'FM');
INSERT INTO "Judge" VALUES (102,'Rakesh Sharma',65,'New Delhi India ','5874612983','LLB.1,BA.LLB',23,'200','yes','High Court',102,'CI');
INSERT INTO "Judge" VALUES (103,'Suresh Kumar',55,'Jaipur Rajasthan India','6593247851','BCA,BBA',13,'103','yes','High Court',103,'CI');
INSERT INTO "Judge" VALUES (104,'Mukesh Yadav',50,'New Delhi','8574612396','BCA,BBA,LLB',15,'50','alive','District Court',104,'CI');
INSERT INTO "Judge" VALUES (105,'Mohit Kumar Gupta',60,'Kanpur Uttar Pradesh India','5241639875','BBA.LLB,BCA',16,'50','alive','Supreme Court',105,'CI');
INSERT INTO "Judge" VALUES (106,'Roshan Singh Sodhi',56,'Ludhiana Punjab India ','6598234598','BBA,BALLB',23,'126','alive','Supreme Court',106,'CI');
INSERT INTO "Judge" VALUES (107,'Hardik Singh Bisht',60,'Dehradun Uttarakhand India','4569871549','BBA,BCA,LLB',14,'120','alive','High Court',107,'CI');
INSERT INTO "Judge" VALUES (108,'Himanshu Singh Deoria',56,'Ranchi Jharkhand India','5648912547','BBA,BCA,LLB',25,'250','alive','Supreme Court',108,'CI');
INSERT INTO "Judge" VALUES (109,'Harsh Kumar Sharma',58,'CHandigarh Haryana India ','6598154263','BBA,BCA',32,'230','alive','High Court',109,'CI');
INSERT INTO "Judge" VALUES (110,'Mokshraj Singh Bhadoria',62,'Bhopal Madhya Pradesh India ','9823257540','BCA,BBA,LLB',23,'163','alive','Supreme Court',110,'CR');
INSERT INTO "Lawyer" VALUES (1,'Aditya Gupta',20,'rani chock','1565145954','LLB',2,1,1,NULL,NULL,0,'CI');
INSERT INTO "Lawyer" VALUES (101,'Aditya Gupta',20,'rani chock','1565145954','LLB',2,1,1,'alive','supreme court',101,'CI');
INSERT INTO "Lawyer" VALUES (102,'prashant singh',68,'bihar','9233438283','LLB',25,1001,1000,'ALIVE','SUPREME COURT',102,'CI');
INSERT INTO "case_schedule" VALUES (1,1,101,NULL,'2026-01-02','17:26:24','17:26:34','Completed',NULL,'2025-11-14 02:36:29','2025-11-14 11:56:34','https://meet.google.com/hmi-rkgg-bge','00:00:08','00:00:02');
INSERT INTO "case_schedule" VALUES (2,2,101,NULL,'2026-01-02','17:30:24',NULL,'Scheduled',NULL,'2025-11-14 07:39:02','2025-11-14 07:39:02',NULL,NULL,NULL);
INSERT INTO "case_schedule" VALUES (3,3,101,NULL,'2026-01-03','18:30:30',NULL,'Scheduled',NULL,'2025-11-14 07:39:03','2025-11-14 07:39:03',NULL,NULL,NULL);
INSERT INTO "case_schedule" VALUES (4,4,101,NULL,'2026-02-19','13:02:06','13:02:08','Completed',NULL,'2025-11-14 07:39:04','2026-02-19 07:32:08',NULL,'00:00:02','00:00:00');
INSERT INTO "case_schedule" VALUES (5,1,101,NULL,'2026-02-19','01:58:26','01:58:28','Completed',NULL,0,'2026-02-18 20:28:28','https://meet.google.com/hmi-rkgg-bge','00:00:02','00:00:00');
INSERT INTO "clerk" VALUES ('101','Aditya',25,'Dandi','9311797158','13-Nov-25','101');
COMMIT;
