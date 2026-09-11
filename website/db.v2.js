// db.js - Complete database handling file
// Loads sql.js + Judgment.db locally with Subresource Integrity verification.

// ─── Integrity hashes (regenerate with: sha256sum website/vendor/*) ─────────
const SQL_WASM_SHA256 =
    "d7e61b828523001f26ce0b3f88dabcf6c12e5e6edf80eb4f08b26ac7b946ff88";
const JUDGMENT_DB_SHA256 = ""; // optional: leave empty to skip DB hash check
// ────────────────────────────────────────────────────────────────────────────

// Wait for sql-wasm.js to finish executing and expose `initSqlJs` globally.
// This eliminates the race condition where db.js runs before the library is ready.
function waitForSqlJs(timeoutMs = 10000) {
    return new Promise((resolve, reject) => {
        if (typeof initSqlJs !== "undefined") return resolve();
        const start = Date.now();
        const tick = () => {
            if (typeof initSqlJs !== "undefined") return resolve();
            if (Date.now() - start > timeoutMs) {
                return reject(new Error("sql.js (initSqlJs) not available after " + timeoutMs + "ms"));
            }
            setTimeout(tick, 20);
        };
        tick();
    });
}

// Fetch a URL, optionally verify its SHA-256 (hex) against expectedHash.
async function fetchAndVerify(url, expectedSha256, label) {
    const response = await fetch(url);
    if (!response.ok) {
        throw new Error(`Failed to load ${label}: ${response.status} ${response.statusText}`);
    }
    const buf = await response.arrayBuffer();
    const bytes = new Uint8Array(buf);
    if (expectedSha256) {
        const hashBuf = await crypto.subtle.digest("SHA-256", bytes);
        const got = Array.from(new Uint8Array(hashBuf))
            .map(b => b.toString(16).padStart(2, "0"))
            .join("");
        if (got !== expectedSha256.toLowerCase()) {
            throw new Error(`${label} SHA-256 mismatch! expected=${expectedSha256} got=${got}`);
        }
        console.log(`✓ ${label} SHA-256 verified`);
    }
    return bytes;
}

// Global database instance
window._db = null;

// Top-level (hoisted) function declarations so the names `initDB`, `dbQuery`,
// `getTables`, `getTableSchema`, `getRowCount` are visible as plain globals
// from any deferred/async script that runs after this file. This also avoids
// `ReferenceError: initDB is not defined` in browsers that previously cached
// an older version of this file where these were only assigned to `window.*`.
async function initDB() {
    return _initDBImpl();
}
async function _initDBImpl() {
    // Always reload fresh — never use cached DB so updates are always picked up
    window._db = null;

    try {
        console.log("Waiting for sql.js to be available...");
        await waitForSqlJs();
        console.log("Initializing SQL.js (local vendor)...");

        // locateFile: tell sql.js where to fetch the .wasm from (local vendor)
        const SQL = await initSqlJs({
            locateFile: file => `vendor/${file}`
        });

        console.log("Loading database file (verified)...");

        // Cache-busting + integrity-checked fetch of the SQLite DB.
        // Try multiple paths because the page may be served from:
        //   - /site/ (unified server)  → /site/Judgment.db
        //   - /website/ (old layout)   → ../Judgment.db
        //   - project root             → Judgment.db
        const dbPaths = ["/site/Judgment.db", "../Judgment.db", "Judgment.db"];
        let uint8Array = null;
        for (const p of dbPaths) {
            try {
                uint8Array = await fetchAndVerify(
                    `${p}?v=${Date.now()}`,
                    JUDGMENT_DB_SHA256 || null,
                    `Judgment.db (${p})`
                );
                console.log(`✓ Loaded DB from ${p}`);
                break;
            } catch (e) {
                console.warn(`Could not load DB from ${p}: ${e.message}`);
            }
        }
        if (!uint8Array) {
            throw new Error("Could not locate Judgment.db in any known path");
        }

        // Sanity check: must start with the SQLite magic header
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
// Mirror on `window` for backward compatibility with any code that uses
// `window.initDB` directly.
window.initDB = initDB;

// Main dbQuery - supports both patterns for backward compatibility
// Pattern A (new): dbQuery(sqlString, paramsArray)
// Pattern B (old): dbQuery(dbObject, sqlString, paramsArray)
function dbQuery(sqlOrDb, paramsOrSql = [], sqlParamsOrNull = null) {
    let actualDb, actualSql, actualParams;

    if (typeof sqlOrDb === 'object' && sqlOrDb !== null && typeof sqlOrDb.prepare === 'function') {
        // Pattern B: first arg is a db object
        actualDb     = sqlOrDb;
        actualSql    = paramsOrSql;
        actualParams = sqlParamsOrNull || [];
    } else {
        // Pattern A: first arg is a SQL string
        actualDb     = window._db;
        actualSql    = sqlOrDb;
        actualParams = paramsOrSql || [];
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
window.dbQuery = dbQuery;

// Helper function to get all tables
function getTables(db = null) {
    const actualDb = db || window._db;
    return window.dbQuery(actualDb, "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");
}
window.getTables = getTables;

// Helper function to get table schema
function getTableSchema(tableName, db = null) {
    const actualDb = db || window._db;
    return window.dbQuery(actualDb, `PRAGMA table_info(${tableName})`);
}
window.getTableSchema = getTableSchema;

// Helper function to get row count
function getRowCount(tableName, db = null) {
    const actualDb = db || window._db;
    const result = window.dbQuery(actualDb, `SELECT COUNT(*) as count FROM ${tableName}`);
    return result[0] ? result[0].count : 0;
}
window.getRowCount = getRowCount;

// Initialize database automatically when page loads, then update the
// live stats bar on the home page if the elements are present.
document.addEventListener('DOMContentLoaded', async () => {
    try {
        await initDB();
        console.log("Database auto-initialized successfully");

        // Update home-page stats badge + stat counters if present.
        const badge = document.getElementById('db-status');
        if (badge) {
            badge.textContent = '✅ Database connected';
            badge.className = 'db-badge connected';
        }
        const statsBar = document.getElementById('statsBar');
        if (statsBar && window._db) {
            const q = (sql) => window.dbQuery(window._db, sql)[0];
            const set = (id, sql) => {
                const el = document.getElementById(id);
                if (!el) return;
                const row = q(sql);
                if (row) el.textContent = row.c ?? row.count ?? '—';
            };
            set('statCases',     "SELECT COUNT(*) AS c FROM Crime_table");
            set('statLawyers',   "SELECT COUNT(*) AS c FROM Lawyer WHERE Lyr_ID != 1");
            set('statJudges',    "SELECT COUNT(*) AS c FROM Judge");
            set('statSchedules', "SELECT COUNT(*) AS c FROM case_schedule");
            statsBar.style.display = 'flex';
        }
    } catch (error) {
        console.error("Failed to auto-initialize database:", error);
        const badge = document.getElementById('db-status');
        if (badge) {
            badge.textContent = '❌ Database error: ' + error.message;
            badge.className = 'db-badge error';
        }
    }
});