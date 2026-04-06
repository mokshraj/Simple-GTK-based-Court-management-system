// db.js - Complete database handling file

// Global database instance
window._db = null;

window.initDB = async function initDB() {
    // Return existing database if already initialized
    if (window._db && typeof window._db.prepare === 'function') {
        console.log("Returning existing database connection");
        return window._db;
    }

    try {
        console.log("Initializing SQL.js...");
        
        // Initialize SQL.js
        const SQL = await initSqlJs({
            locateFile: file => `https://unpkg.com/sql.js@1.10.3/dist/${file}`
        });

        console.log("Loading database file...");
        
        // Load the database file
        const response = await fetch("../Judgment.db");
        console.log("Response status:", response.status);

        if (!response.ok) {
            throw new Error(`Failed to load database: ${response.status} ${response.statusText}`);
        }

        const arrayBuffer = await response.arrayBuffer();
        console.log("Buffer size:", arrayBuffer.byteLength, "bytes");

        const uint8Array = new Uint8Array(arrayBuffer);
        
        // Verify it's a SQLite database
        const header = String.fromCharCode(...uint8Array.slice(0, 16));
        console.log("File header:", header);
        
        if (!header.startsWith("SQLite format 3")) {
            throw new Error("File is not a valid SQLite database");
        }

        // Create the database
        window._db = new SQL.Database(uint8Array);
        console.log("Database object created:", typeof window._db);
        console.log("Database prepare function:", typeof window._db.prepare);
        
        return window._db;
    } catch (error) {
        console.error("Database initialization error:", error);
        throw error;
    }
}

// Main dbQuery - supports both patterns for backward compatibility
window.dbQuery = function dbQuery(sql, params = [], dbParam = null) {
    // Handle the old pattern: dbQuery(db, sql, params)
    let actualDb = window._db;
    let actualSql = sql;
    let actualParams = params;
    
    if (dbParam !== null || (typeof sql === 'object' && sql !== null && typeof sql.prepare === 'function')) {
        // Old pattern: first argument is the database object
        actualDb = (typeof sql === 'object' && sql !== null && typeof sql.prepare === 'function') ? sql : dbParam;
        actualSql = (typeof sql === 'object' && sql !== null && typeof sql.prepare === 'function') ? params : sql;
        actualParams = (typeof sql === 'object' && sql !== null && typeof sql.prepare === 'function') ? (params || []) : (dbParam || []);
    }
    
    // Make sure database is initialized
    if (!actualDb || typeof actualDb.prepare !== 'function') {
        console.error("Database object is invalid. Type:", typeof actualDb);
        throw new Error("Database not properly initialized. Call initDB() first.");
    }
    
    let stmt = null;
    try {
        stmt = actualDb.prepare(actualSql);
        stmt.bind(actualParams);
        const rows = [];
        while (stmt.step()) {
            rows.push(stmt.getAsObject());
        }
        return rows;
    } catch (error) {
        console.error("Query error:", error);
        console.error("SQL:", actualSql);
        console.error("Params:", actualParams);
        throw error;
    } finally {
        if (stmt) {
            stmt.free();
        }
    }
}

// Helper function to get all tables
window.getTables = function getTables(db = null) {
    const actualDb = db || window._db;
    return window.dbQuery(actualDb, "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");
}

// Helper function to get table schema
window.getTableSchema = function getTableSchema(tableName, db = null) {
    const actualDb = db || window._db;
    return window.dbQuery(actualDb, `PRAGMA table_info(${tableName})`);
}

// Helper function to get row count
window.getRowCount = function getRowCount(tableName, db = null) {
    const actualDb = db || window._db;
    const result = window.dbQuery(actualDb, `SELECT COUNT(*) as count FROM ${tableName}`);
    return result[0] ? result[0].count : 0;
}

// Initialize database automatically when page loads
document.addEventListener('DOMContentLoaded', async () => {
    try {
        await window.initDB();
        console.log("Database auto-initialized successfully");
    } catch (error) {
        console.error("Failed to auto-initialize database:", error);
    }
});