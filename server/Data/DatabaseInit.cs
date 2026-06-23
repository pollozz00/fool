using Dapper;
using Microsoft.Data.SqlClient;

namespace FoolWebAPI.Data
{
    public class DatabaseInit
    {
        private readonly string _connStr;

        public DatabaseInit(string connStr) {
            _connStr = connStr;
        }

        public void Init()
        {
            try
            {
                using var conn = new SqlConnection(_connStr);

                conn.Open();

                conn.Execute(@"
                IF NOT EXISTS (SELECT name FROM sys.databases WHERE name = 'FoolDB') 
                CREATE DATABASE FoolDB
                ");

                conn.ChangeDatabase("FoolDB");

                conn.Execute(@"
                IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='Players' AND xtype='U') 
                CREATE TABLE Players(
                    id INT PRIMARY KEY IDENTITY(1,1),
                    username NVARCHAR(30) NOT NULL UNIQUE,
                    password NVARCHAR(255) NOT NULL,
                    display_name NVARCHAR(100) NOT NULL,
                    wins INT DEFAULT 0,
                    losses INT DEFAULT 0,
                    total_score INT DEFAULT 0,
                    games_played INT DEFAULT 0
                )
                ");

                conn.Execute(@"
                IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='GameHistory' AND xtype='U') 
                CREATE TABLE GameHistory(
                    id INT PRIMARY KEY IDENTITY(1,1),
                    player_id INT FOREIGN KEY REFERENCES Players(id),
                    score INT DEFAULT 0,
                    result NVARCHAR(10),
                    played_at DATE DEFAULT GETDATE()
                )
                ");

                var count = conn.ExecuteScalar<int>("SELECT COUNT(*) FROM Players");
                if (count == 0)
                {
                    var hash = BCrypt.Net.BCrypt.HashPassword("password123");

                    conn.Execute(@"
                    INSERT INTO Players (username, password, display_name, wins, losses, total_score, games_played)
                    VALUES
                    ('alex99',    @h, 'alex999',   8, 3, 215, 11),
                    ('martaK',    @h, 'Marta',     6, 5, 140, 11),
                    ('ironvex',   @h, 'ironvex',   1, 4,  10,  5),
                    ('frostnova', @h, 'Frostnova', 5, 3, 120,  8),
                    ('darkorbit', @h, 'darkorbit', 3, 5,  45,  8)",
                    new { h = hash }
                    );

                    var alex = conn.ExecuteScalar<int>("SELECT id FROM Players WHERE username='alex99'");
                    var marta = conn.ExecuteScalar<int>("SELECT id FROM Players WHERE username='martaK'");
                    var ironvex = conn.ExecuteScalar<int>("SELECT id FROM Players WHERE username='ironvex'");
                    var frost = conn.ExecuteScalar<int>("SELECT id FROM Players WHERE username='frostnova'");
                    var dark = conn.ExecuteScalar<int>("SELECT id FROM Players WHERE username='darkorbit'");

                    //alex99
                    conn.Execute(@"INSERT INTO GameHistory (player_id, score, result, played_at) VALUES
                    (@id, 50, 'win',  '2026-05-01 10:15:00'),
                    (@id, 25, 'win',  '2026-05-01 11:30:00'),
                    (@id, 10, 'win',  '2026-05-02 09:00:00'),
                    (@id,  0, 'loss', '2026-05-02 10:20:00'),
                    (@id, 50, 'win',  '2026-05-03 14:00:00'),
                    (@id, 25, 'win',  '2026-05-04 16:10:00'),
                    (@id,  0, 'loss', '2026-05-05 09:45:00'),
                    (@id, 10, 'win',  '2026-05-05 11:00:00'),
                    (@id, 25, 'win',  '2026-05-06 13:30:00'),
                    (@id,  0, 'loss', '2026-05-07 10:00:00'),
                    (@id, 20, 'win',  '2026-05-08 15:00:00')",
                    new { id = alex });

                    //martaK
                    conn.Execute(@"INSERT INTO GameHistory (player_id, score, result, played_at) VALUES
                    (@id, 25, 'win',  '2026-05-01 12:00:00'),
                    (@id,  0, 'loss', '2026-05-02 08:30:00'),
                    (@id, 10, 'win',  '2026-05-02 14:00:00'),
                    (@id, 25, 'win',  '2026-05-03 10:00:00'),
                    (@id,  0, 'loss', '2026-05-04 09:00:00'),
                    (@id, 50, 'win',  '2026-05-05 11:30:00'),
                    (@id,  0, 'loss', '2026-05-06 13:00:00'),
                    (@id, 10, 'win',  '2026-05-06 15:00:00'),
                    (@id,  0, 'loss', '2026-05-07 09:30:00'),
                    (@id, 10, 'win',  '2026-05-07 11:00:00'),
                    (@id,  0, 'loss', '2026-05-08 10:00:00')",
                    new { id = marta });

                    //ironvex
                    conn.Execute(@"INSERT INTO GameHistory (player_id, score, result, played_at) VALUES
                    (@id, 10, 'win',  '2026-05-03 11:00:00'),
                    (@id,  0, 'loss', '2026-05-04 12:00:00'),
                    (@id,  0, 'loss', '2026-05-05 13:00:00'),
                    (@id,  0, 'loss', '2026-05-06 09:00:00'),
                    (@id,  0, 'loss', '2026-05-07 10:00:00')",
                    new { id = ironvex });

                    //frostnova
                    conn.Execute(@"INSERT INTO GameHistory (player_id, score, result, played_at) VALUES
                    (@id, 25, 'win',  '2026-05-02 09:00:00'),
                    (@id,  0, 'loss', '2026-05-02 11:00:00'),
                    (@id, 50, 'win',  '2026-05-03 14:00:00'),
                    (@id,  0, 'loss', '2026-05-04 10:00:00'),
                    (@id, 25, 'win',  '2026-05-05 12:00:00'),
                    (@id, 10, 'win',  '2026-05-06 11:00:00'),
                    (@id,  0, 'loss', '2026-05-07 09:00:00'),
                    (@id, 10, 'win',  '2026-05-08 13:00:00')",
                    new { id = frost });

                    //darkorbit
                    conn.Execute(@"INSERT INTO GameHistory (player_id, score, result, played_at) VALUES
                    (@id, 25, 'win',  '2026-05-01 10:00:00'),
                    (@id,  0, 'loss', '2026-05-02 11:00:00'),
                    (@id,  0, 'loss', '2026-05-03 12:00:00'),
                    (@id, 10, 'win',  '2026-05-04 13:00:00'),
                    (@id,  0, 'loss', '2026-05-05 14:00:00'),
                    (@id, 10, 'win',  '2026-05-06 15:00:00'),
                    (@id,  0, 'loss', '2026-05-07 09:00:00'),
                    (@id,  0, 'loss', '2026-05-08 10:00:00')",
                    new { id = dark });

                }
            }
            catch (Exception ex) {
                Console.WriteLine($"Помилка підключення к БД: {ex.Message}");
            }
        }

    }
}
