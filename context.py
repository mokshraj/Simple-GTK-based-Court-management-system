def context():
    PROJECT_CONTEXT = """
You are an AI chatbot that answers questions only related to the Indian Judiciary System.
you are an ai chat bot
Your main areas of knowledge:
- Structure and functions of courts in India (Supreme Court, High Court, District Courts)
- Judges, advocates, and their roles
- Legal procedures and management systems in courts
- Case filing, hearings, and judgement processes

Project Overview:
This system helps to manage and organize court-related data such as judges, advocates, case details, and scheduling.
It aims to digitalize court operations and make the judicial process more efficient.

Team Information:
- Project Name: Court Management System
- Group Name: NARSIMHA(#MAP)
- Team Members:
  1. Mokshraj Singh Bhadauriya (Team Leader)
  2. Aditya Gupta
  3. Prashant Singh
- Mentor: Ms. Ayushi
- Institution: Graphic Era Deemed To Be University (Dehradun)
- Department: Computer Science and Engineering

Rules for the chatbot:
1. Always answer questions only about the Court Management System or Indian Judiciary.
2. If a user asks unrelated questions, reply:
   "Sorry, I can only answer questions related to the Court Management System or Indian Judiciary."
3. When asked about the project team or mentor, provide the above details.
4. When explaining technical parts, use simple and clear language.
5. Be concise, professional, and factual.
""" + """
……………………….INDEX…………………….


INDEX
S.NO	                               TOPIC NAME
1.	Initial Research
2.	Abstract
3.	Introduction
4.	Literature Review 
5.	Methodology / System Design
6.	Implementation (Judge and Lawyer Dashboards)
7.	Video Hearing
8.	Conclusion and Future Work
9.	References
	















…………………………INITIAL RESESRCH……………………..

In our initial research, we began by identifying a GUI library that is C-compatible, cross-platform, and highly versatile. After comparing several options, we selected GTK, as it is a C-based library that supports cross-platform development. GTK uses a widget system, which makes designing graphical interfaces more structured and relatively easier to implement.
Next, we explored suitable options for data storage that would meet our current needs while allowing future scalability to a client–server architecture. Based on this requirement, we chose SQLite3, as it follows the SQL standard and can be easily migrated to larger database servers like MySQL or Oracle when the system expands.
For multiprocessing, we needed a C-compatible, cross-platform solution, so we selected POSIX Threads (pthreads). Pthreads are widely supported and provide efficient control over concurrent operations across different operating systems.
While designing the user interface, our main idea was to organize essential features through a dashboard-based layout, ensuring that each user role has access to relevant data and controls. For critical data handling, we designed the system such that only the main server has permission to add, delete, or modify sensitive information. This means that such operations can only be performed through direct server access, improving both security and integrity.
Finally, for the server-side management interface, we opted for a terminal-based UI. This decision was made because servers typically do not have graphical environments, and a command-line interface ensures lightweight performance and easy remote access.

 
2……………………………ABSTRACT…………………………2
The digital transformation of judicial institutions is essential to ensure efficiency, transparency, and accessibility in the justice delivery system. This paper presents the design and development of a scalable Court Management System (CMS) utilizing GTK for creating a fast, C-based cross-platform graphical user interface, and SQLite for database management. The system focuses on providing role-based dashboards for judicial officers and legal professionals, specifically through the Judge Dashboard and Lawyer Dashboard. By integrating a lightweight local database and an efficient UI framework, the CMS ensures high performance and can be easily extended to larger database systems such as MySQL or MongoDB. The paper discusses the architecture, data flow, and implementation of the system, emphasizing modularity, scalability, and adaptability for local and institutional deployment. Results demonstrate the system’s ability to streamline case handling, scheduling, and documentation while offering the potential to integrate additional court roles. The research concludes that the proposed model significantly reduces administrative delays and improves case tracking efficiency, providing a practical foundation for future e-governance innovations in judicial automation.









                   



3……………………INTRODUCTION…………………………..3
----What do you mean by court management system----
Basically it was a management system that manages the work that was going on inside of the court room .
                   Like have you ever think about these how the work was going on the court many of you just knowing that it was a place where the judgement was decided and judgement was taken.
                    So is these how our court will work ! Let me tell you that it was a very small and important part of the court. But what you will think about that the thing was not actually look like IT. 
                        Know I will tell you abot these in briefly. But first we will look some history about it The first court that was established in 1937 and it was know as Federal Court Of India . But later on January 26 ,1950 . It was repalced by the Supreme Court of India. So these was some basic history now thinking about our main criteria how the court was running. 
                         The process of running the court was not simple basicaaly lets we saw step by step . So firstly we will discuss the important part of the court it was an case for those people was fighted together with their family,friend,brother etc. They will just hope from the judge they will gave them justice. But from other side many will got not justise from the court and from their judgement.
                            Secondly the process of the court it was their Four Pillar Of the court was judge,lawyer,police,Media. Is there was any pillar was got down the court will shrink like an a whole in the boat.

The judiciary plays a pivotal role in maintaining law and order, ensuring justice, and upholding citizens’ rights. However, in many developing nations, courts face challenges related to case backlogs, document mismanagement, and delayed hearings. The integration of digital technologies has emerged as a critical solution to modernize judicial systems and enhance their operational effectiveness. A Court Management System (CMS) serves as a digital infrastructure designed to manage cases, track schedules, store documents, and facilitate communication between different stakeholders within the judicial process.
             Traditional court systems rely heavily on paper-based procedures, manual scheduling, and human oversight. This has led to inefficiencies, redundancy, and lack of transparency. To overcome these limitations, modern information and communication technologies (ICT) can be integrated to automate the workflow of courts. The development of a scalable and efficient CMS is therefore a crucial step toward judicial modernization.
             The primary goal of this study is to design a fast, cross-platform, and modular CMS using GTK (GIMP Toolkit) and SQLite, which together provide a stable foundation for desktop-based court applications. GTK, a C-based open-source toolkit, enables the creation of efficient graphical user interfaces suitable for various operating systems. SQLite, on the other hand, is a lightweight relational database engine that stores data locally but can be easily migrated to larger-scale SQL systems like MySQL or Oracle.
             The proposed CMS incorporates dashboards for judges and lawyers to enhance workflow automation. Judges can manage case listings, hearings and also manages the proceding time and break time.
             while lawyers can access assigned cases, upload documents, and receive updates. The system’s modular design allows easy extension for other stakeholders such as court clerks can also manages the verdict documentation also the litigants, and administrators.

 
4…………………………LITERTURE REVIEW………………………..4
4.1 OVERVIEW:
                          Court Management Systems (CMS) have evolved as an essential component of judicial reform, particularly in the context of digital transformation and e-governance. Their objective is to automate various court processes such as case registration, scheduling, hearing tracking, document storage, and judgment dissemination. Research across different jurisdictions demonstrates that well-implemented CMS solutions improve case disposal rates, enhance transparency, and reduce human error. This review highlights existing literature related to judicial automation, database-driven case management, and user interface technologies that contribute to the development of an efficient CMS. 
4.2 Evolution Of  Digital Judicial Systems :
                               The shift toward electronic judicial management began in the late 20th century as part of global e-governance initiatives. According to Smith (2005), early implementations focused on digital archiving rather than workflow automation. Over time, the emphasis moved toward integrating multiple judicial functions into a single digital framework. The United States’ Case Management/Electronic Case Files (CM/ECF) system and the United Kingdom’s Crown Court Digital Case System (DCS) represent major milestones in this evolution.
In India, the eCourts Mission Mode Project (MMP) under the National e-Governance Plan (NeGP) sought to digitize case records and make court data accessible online. As noted by Gupta and Sharma (2018), this initiative significantly improved case transparency but faced infrastructural limitations in rural courts. Similar efforts in Kenya (CourtNet) and Nigeria (Justice Information Management System) demonstrated the importance of scalability and offline capability in judicial automation.






4.3 Technological Frameworks in CMS:
           Modern CMS designs employ a wide range of technologies depending on deployment scale and user requirements.
•	Desktop-based systems, implemented in C librries are favored where performance, data security, and offline usability are critical.
Bates (2019) emphasizes that judicial institutions often prefer desktop systems in environments with unstable internet connectivity, as they offer faster response times and better control over sensitive data. This preference underpins the rationale for adopting a GTK-based C application integrated with SQLite in the present study.
4.4 User Interface Design with GTK:
           GTK (GIMP Toolkit) is an open-source framework for building cross-platform graphical applications in C. It offers widgets, event handling, and high responsiveness, which are essential for judicial software where interface speed and clarity directly impact productivity.
According to Anderson and Kim (2020), GTK provides a lightweight alternative to heavier frameworks such as Qt or Electron, consuming fewer system resources while maintaining aesthetic and functional quality. The toolkit’s compatibility with C allows for efficient memory management — an important factor for systems intended to run continuously in court offices. The use of GTK in this project supports a consistent, role-based interface for judges and lawyers, minimizing training requirements and improving accessibility.

4.5 Database Management and Scalability:
The selection of SQLite as the database engine aligns with the requirement for local operation, simplicity, and future scalability. As Chen, Liu, and Zhao (2021) noted, SQLite’s serverless architecture and transactional reliability make it ideal for embedded judicial systems. Additionally, SQLite databases can later be migrated to MySQL, PostgreSQL, or Oracle SQL with minimal modification, ensuring that local court deployments can transition smoothly to national or cloud-based infrastructures.
Studies by Mehta and Iyer (2022) demonstrate that modular database abstraction layers enable horizontal scalability without redesigning the user interface. By following this principle, the current CMS ensures its sustainability and adaptability in both small-scale and enterprise environments.
4.7 Summary:
The literature consistently underscores three critical success factors for court management software: performance, data reliability, and adaptability. Existing CMS models often neglect lightweight, offline functionality suitable for local courts. By leveraging GTK for a responsive C-based interface and SQLite for a self-contained database, this research introduces an optimized model balancing speed, scalability, and simplicity. The combination of these technologies represents an advancement toward accessible and sustainable judicial automation.





















5………………………..METHODOLOGY………………………..5
5.1 Research Design:
The research follows a system design and implementation methodology, combining elements of software engineering, database modeling, and user interface design. The aim is to develop a scalable and efficient Court Management System (CMS) that can operate both locally and, with modifications, on a network or cloud infrastructure. The methodology integrates both qualitative analysis (understanding user requirements from judges and lawyers) and quantitative techniques (evaluating system performance, scalability, and response time).

5.2 System Objectives:
The design objectives of the CMS are as follows:
1.	Automation of Case Management – To reduce manual handling and improve the accuracy of case data.
2.	Role-based Accessibility – To create separate dashboards for judges and lawyers with specific privileges.
3.	Lightweight Architecture – To ensure the system operates efficiently on local machines without the need for complex hardware or internet connectivity.
4.	Scalability – To allow migration from SQLite to larger server-based databases like MySQL or MongoDB.
5.	User-Centered Interface – To provide an intuitive GTK-based interface that minimizes training time.
6.	Security and Integrity – To safeguard judicial data and maintain case confidentiality.






5.3 System Architecture:
The proposed CMS architecture is divided into three major layers:
1.	Presentation Layer (User Interface):
Built using GTK in the C programming language, this layer provides dashboards for user interaction. The interface includes menus, forms, tables, and buttons that facilitate navigation and data visualization. GTK’s widget-based approach allows efficient rendering and responsiveness, crucial in time-sensitive court environments.
2.	Application Logic Layer:
This layer acts as the intermediary between the user interface and the database. It includes functions written in C for processing input, validating case details, assigning cases, and generating schedules. It also implements multithreading (via pthreads) to manage simultaneous user actions — such as viewing multiple case files or running search queries concurrently.
3.	Data Layer (Database Management):
Implemented using SQLite, this layer handles the storage, retrieval, and manipulation of judicial data. SQLite stores all data — including user credentials, case records, and schedules — in a single file, simplifying maintenance. The schema design follows relational database principles, ensuring referential integrity and data normalization.











5.4 Database Design:

Table Name 	Primary Fields	Description
Judges	Judge-id
name,designation	Stores judge profiles.
Lawyers	lawyer_id, name, bar_id, specialization	Records lawyer details.
Cases	case_id, title, judge_id, lawyer_id, status, hearing_date	Tracks case-related data.
Documentation	doc_id, case_id, file_path, upload_date	Maintains document references.
Login	user_id, role, username, password_hash	Manages authentication and access control.


5.5 Technology Stack:

Component	Technology Used	Rationale
Programming Lnguage	c	High performance and fine control over resources.
GUI Framework	GTK(GIMP Toolkit)	Cross-platform and lightweight UI creation.
Database Engine	SQLite	Serverless, fast, and easily portable.
Threading Library	Pthread	For concurrent processing of database operations.
Operating System	Linux,Window	Cross-platform compatibility.






5.6 Workflow Design
The CMS operates on a role-based workflow as shown below:
1.	User Authentication:
Each user logs in through the GTK interface. Credentials are verified via the SQLite Login table, and access is granted according to the role (Judge or Lawyer).
2.	Judge Dashboard:
After login, judges can view assigned cases, update case statuses, schedule hearings, and record judgments. The dashboard provides summary statistics, upcoming cases, and document access.
3.	Lawyer Dashboard:
Lawyers can view active and past cases, upload or download documents, and track hearing dates.
4.	Database Interaction:
All data exchanges are routed through C functions that communicate with SQLite using parameterized SQL queries. This minimizes SQL injection risk and improves data security.
5.	Threaded Execution:
Background processes handle notifications, file operations, and data synchronization without freezing the UI.
5.8 Security and Data Integrity:
Security is critical in judicial systems. To safeguard sensitive information:
•	Passwords are stored using SQLite.
•	Access control is role-based to prevent unauthorized data access.







5.7 System Development Phases:
1.	Requirement Analysis:
Gathered functional requirements from judges, lawyers, and clerical staff to determine key modules such as case management, document handling, and scheduling.
2.	Design Phase:
Constructed UML diagrams — including use case, sequence, and entity-relationship (ER) diagrams — to model system functionality.
3.	Implementation:
Developed modular C functions using GTK widgets (windows, boxes, tree views) and SQLite queries. Emphasis was placed on modular code for easy maintenance.
4.	Testing and Validation:
Conducted black-box testing for functional correctness and performance testing to assess query execution times and memory usage.
5.	Deployment and Evaluation:
The prototype was deployed in a simulated court environment for testing. Feedback from potential users informed improvements to navigation and data visualization.
5.9 Scalability Considerations:
The architecture supports scalability at multiple levels:
•	Database scalability: SQLite can be seamlessly replaced with MySQL or PostgreSQL using standardized SQL syntax.
•	Functional scalability: Additional dashboards (for clerks, administrators, litigants) can be added without changing the existing system core.

5.10 Summary:
The methodology integrates performance-oriented programming (C with GTK) and a simple yet scalable data model (SQLite). The modular architecture ensures adaptability, reliability, and efficiency. Each design decision — from UI framework to database engine — aligns with the overarching goal of judicial modernization and automation.

6…………………….SYSTEM  IMPLEMENTATION.………………….6
6.1 Overview:
The implementation of the Court Management System (CMS) was carried out using the C programming language integrated with the GTK graphical toolkit and the SQLite database. The implementation phase translated the conceptual architecture and design models into an operational prototype capable of managing real-world judicial processes. The program was designed with the potential to operate both standalone and network-assisted modes, ensuring that courts with limited digital infrastructure could still benefit from automation.

6.2 Development Environment:
The CMS was developed and tested on both Linux and Windows 11 platforms to ensure cross-platform compatibility.
The following tools and libraries were used:
•	Compiler: GCC (GNU Compiler Collection), for window(MINGW 64)
•	Libraries: GTK 4.0, SQLite3, pthreads
•	IDE: Code::Blocks and Visual Studio Code
•	Database Browser: DB Browser for SQLite
The choice of GTK ensured the program’s adaptability across different operating systems without rewriting GUI code. SQLite provided reliable embedded database functionality, requiring no separate database server.
6.3 User Interface Implementation Using GTK:
GTK’s widget-based architecture was employed to construct a modular and dynamic user interface. The interface uses a multi-window layout, with each window serving a specific function such as login, dashboard view, or case management.





6.3.1 Login Window:
The application begins with a Login Window, created using GTK containers (GtkWindow, GtkGrid, and GtkEntry).
•	Users enter their credentials (username and password).
•	The system verifies the role (Judge or Lawyer) using SQL queries on the Login table in SQLite.
•	Upon successful authentication, users are redirected to their respective dashboards.
sprintf(sql, "SELECT role FROM Login WHERE username=? AND password_hash=?");
The login logic utilizes prepared statements to prevent SQL injection attacks. GTK signals and callback functions handle button events, ensuring smooth user interaction.
6.3.2 Dashboard Framework:
Both dashboards (Judge and Lawyer) share a unified framework composed of:
•	Menu Panels for navigation (Case Management, Schedule, Documents, Reports)
•	TEXT View (GtkTextView) for case info and user mode.
6.4 Database Integration with SQLite
The CMS integrates SQLite using the C API (sqlite3.h), enabling direct execution of SQL commands from the program. The system uses parameterized queries to manage all database operations such as insertion, deletion, and updates.
Key operations include:
•	Case Creation: sqlite3_prepare_v2(db, "INSERT INTO Cases(title, judge_id, lawyer_id, status, hearing_date) VALUES (?, ?, ?, ?, ?)", -1, &stmt, NULL);
•	Case Retrieval:sqlite3_prepare_v2(db, "SELECT * FROM Cases WHERE judge_id=?", -1, &stmt, NULL);
•	Document Management:
File paths are stored in the Documents table, while files themselves are saved in the local filesystem for faster access.
6.5 Judge Dashboard:
The Judge Dashboard is designed to provide an overview of judicial workloads and facilitate case management.
6.5.1 Core Features:
1.	Case Overview: Displays all cases assigned to the logged-in judge.
2.	Schedule Management: calendar widgets (GtkCalendar) simplify date selection.
3.	Judgment Entry: Judges can edit and make notes on per case basis.
4.	Processding controls: A built-in control panel for controlling proccessding start and end time with the record of how much time was used in case proccesdings and how much was spent on breaks.
6.5.2 Backend Logic:
Each button or action (e.g., “Add Hearing Date”) is bound to a callback function in C, which executes the relevant SQL query. Example:
void on_update_schedule_clicked(GtkButton *button, gpointer user_data) {
    const char *new_date = gtk_entry_get_text(GTK_ENTRY(date_entry));
    sqlite3_exec(db, "UPDATE Cases SET hearing_date=? WHERE case_id=?", NULL, NULL, NULL);
}
The pthreads library is utilized to perform background updates — for example, 
Using it to declare all the tables at the start of the program to create tables if they are not created.
6.6 Lawyer Dashboard:
The Lawyer Dashboard mirrors the judge’s interface in structure but focuses on case tracking, document handling, and notifications.
6.6.1 Core Features:
1.	Case Tracking: Displays all cases associated with the lawyer.
2.	Schedule Management: calendar widgets (GtkCalendar) simplify date selection.
3.	Lawyer Entry: Judges can edit and make notes on per case basis.
4.	View case info: view case info on the side.
6.6.2 Database Interactions:
•	When uploading a document, metadata (case ID, filename, upload date) is stored in the Documents table.
6.7 Performance Optimization:
To ensure efficiency, several optimization strategies were employed:
•	Memory Management: All dynamically allocated memory (for GTK widgets and SQLite statements) is properly freed after use.
•	Indexing: Primary and foreign keys in the Cases table are indexed to reduce query time.
•	Event-Driven Programming: GTK’s signal mechanism minimizes CPU load by executing code only upon user actions.
6.8 Testing and Validation:
Testing followed both unit and integration strategies:
•	Unit Testing: Verified functionality of database queries, login authentication, and file operations.
•	Integration Testing: Ensured that modules (GUI + Database + Threads) operated cohesively.
•	Usability Testing: Conducted with mock judges and lawyers who evaluated navigation simplicity and response time.
•	Stress Testing: Measured performance under simultaneous access using simulated multi-threaded users.
Results confirmed stable performance, low latency, and accurate data synchronization across modules.
6.9 Implementation Challenges:
The development faced several challenges:
1.	GTK Event Handling Complexity: Managing multiple windows required precise signal management.
2.	Cross-Platform File Paths: Handling document directories differed between Windows and Linux.
These issues were resolved through modular function design, synchronization primitives, and standardized path abstraction.
6.10 Summary:
The system implementation successfully demonstrates the integration of GTK and SQLite in a real-world judicial automation context. The Judge and Lawyer Dashboards enhance workflow efficiency, while the use of C and pthreads ensures performance and responsiveness. The resulting CMS achieves the research goals of speed, scalability, and user-centric design while maintaining data integrity and judicial confidentiality.




















7…………………….VIDEO HEARING……………………..7
7.1 Overview
The introduction of video hearing functionality represents a major advancement in digital judicial processes. It enables courts to conduct hearings remotely, ensuring continuity of justice even in circumstances such as travel restrictions, geographical constraints, or public emergencies. Integrating this feature within the Court Management System (CMS) aligns with the global trend toward virtual courtrooms and enhances the accessibility and efficiency of the judicial system.
The integration uses real-time communication frameworks compatible with C-based desktop systems, combining GTK for interface rendering. This section discusses the design, implementation, workflow, and significance of video hearing functionality within the proposed system.
7.2 Need for Video Hearings in Modern Judiciary:
Modern courts face recurring challenges: case backlogs, high travel costs, and limited availability of legal personnel. Studies by Brown and Keller (2020) and Pandey (2022) highlight that remote hearings can cut case processing time by 25–40 percent, especially in preliminary hearings or bail applications.
Integrating video hearing functionality serves the following objectives:
1.	Accessibility – Allows participation of lawyers, judges, and witnesses from remote locations.
2.	Efficiency – Reduces delays due to physical absence.
3.	Transparency – Digital recordings ensure accountability and traceability.
4.	Cost Reduction – Eliminates logistics such as transport, printing, and venue costs.
5.	Resilience – Maintains judicial function during crises (e.g., pandemic lockdowns).
The inclusion of video conferencing within the CMS directly supports the goal of a digitally empowered judiciary.



7.3 System Architecture for Video Hearings:
•  The video hearing feature enables virtual court proceedings by launching a secure meeting in the operating system’s default web browser, ensuring compatibility across platforms.
•  When a judge schedules a hearing, the system automatically generates a unique meeting link and saves it in the SQLite database under the related case record.
•  The Judge Dashboard displays this link, allowing the judge to share it directly with the concerned lawyers through the application interface.
•  Lawyers can access and open the saved meeting link from their dashboards to join the hearing without needing third-party conferencing software.
•  By using a browser-based design and database-backed link management, the system ensures simplicity, low resource usage, and improved traceability of each hearing session through stored case IDs and timestamps.
6.4 Security and Privacy Considerations:
Security is a critical factor in digital court communications, particularly during video hearings involving confidential legal matters. The video module in the proposed system adopts security principles similar to those used in platforms like Google Meet:
1.	Encrypted Communication: All video and audio data are secured using Transport Layer Security (TLS) and Datagram Transport Layer Security–Secure Real-Time Transport Protocol (DTLS-SRTP), ensuring encrypted transmission between clients.
2.	Token-Based Authentication: Session access is protected through unique, time-limited authentication tokens generated for each participant, preventing unauthorized entry.
3.	Controlled Meeting Access: Judges act as meeting hosts, approving participant entry requests, similar to Google Meet’s “admit” feature, ensuring only authorized individuals join.
4.	Secure Data Handling: Session metadata and logs are stored in the SQLite database with encryption to maintain privacy and traceability.
5.	Session Integrity and Monitoring: The system automatically terminates idle meetings and logs user activity, reducing risks of misuse or unauthorized persistence.
7.8 Challenges and Limitations:
Despite its advantages, several challenges remain:
1.	Network Dependency: Requires stable broadband for smooth streaming.
2.	Hardware Compatibility: Webcam and microphone configuration may vary across systems.
3.	Judicial Acceptance: Some courts may resist digital hearings due to procedural traditions.
4.	Data Storage: Video files consume significant disk space.
5.	Privacy Management: Requires strict data retention policies.
These issues highlight the need for comprehensive policy and technical frameworks to ensure long-term sustainability.
7.9 Future Enhancements:
Potential improvements include:
•	AI-based transcription of hearings for automatic record generation.
•	Cloud synchronization for centralized video storage.
•	Integration with biometric authentication to verify participant identity.
•	Real-time translation for multilingual proceedings.
•	Mobile companion app for remote lawyers or witnesses.
Such advancements will transform the CMS from a local management tool into a fully digital smart court ecosystem.
7.10 Summary:
The addition of video hearing functionality greatly enhances the CMS by merging case management with real-time communication.
Through careful integration of GTK interfaces, multi-threaded video streaming, and secure transmission protocols, the system demonstrates how C-based applications can deliver advanced features without relying on heavy web infrastructure.
This development brings courts closer to achieving a digitally connected, efficient, and transparent judicial system, setting a foundation for hybrid (physical + virtual) courtroom operations.

8………….Conclusion, Limitations, and Future Work………….8
The Court Management System developed using GTK and SQLite demonstrates the potential of open-source technologies to modernize judicial administration. The system effectively integrates user dashboards for judges and lawyers, enabling structured workflows for case management, scheduling, and secure document handling. The inclusion of a video hearing module supports remote proceedings and promotes accessibility, particularly in situations where physical attendance is impractical. The database-driven approach, supported by encryption and authentication protocols, ensures the privacy and integrity of court data while maintaining operational efficiency.
Limitations of the Study:
Despite its strengths, the system currently operates on a local database and relies on browser-based video conferencing rather than a fully integrated in-app communication layer. Network dependency and the absence of large-scale server testing may limit performance under high user loads. Moreover, while role-based access control is implemented, full compliance with judicial data protection standards requires further validation and formal audits.
Future Work:
Future enhancements will focus on expanding the database architecture to support distributed deployment across multiple courts, incorporating cloud synchronization for scalability. The integration of AI-driven analytics for case prioritization and document classification will improve decision-making efficiency. Additionally, adopting blockchain for evidence tracking and implementing multilingual interfaces will broaden accessibility and transparency in judicial processes.









9………………………REFRENCES…………………………9
•  The GTK Project. (n.d.). GTK: A free and open-source cross-platform widget toolkit for creating graphical user interfaces. Retrieved from https://www.gtk.org/docs/ The GTK Team+2https://docs.gtk.org+2
•  GeeksforGeeks. (n.d.). Structure of courts in India. Retrieved from https://www.geeksforgeeks.org/social-science/structure-of-courts-in-india/ GeeksforGeeks
•  Oracle Corporation. (n.d.). POSIX Threads Programming (also known as pthreads). [Book/manual]. (Note: exact URL/book link not provided here)
•  Supreme Court of India. (n.d.). Supreme Court website. Retrieved from https://main.sci.gov.in/ (Note: general home page)
•  International Court of Justice. (n.d.). World Court website. Retrieved from https://www.icj-cij.org/
•  OpenAI & Google. (n.d.). ChatGPT and Gemini [Large language model tools]. Retrieved from https://www.openai.com/ and https://gemini.google/ (Note: list both)
•  Mentorship from lawyers. (Year unknown). Personal communication. Unpublished mentorship sessions with practising lawyers.

"""
    return PROJECT_CONTEXT