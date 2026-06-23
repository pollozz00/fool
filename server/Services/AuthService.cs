using Dapper;
using FoolWebAPI.DTOs;
using FoolWebAPI.Models;
using Microsoft.Data.SqlClient;
namespace FoolWebAPI.Services
{
    public class AuthService
    {
        private readonly string _connStr;

        public AuthService(string connStr)
        {
            _connStr = connStr;
        }

        private SqlConnection GetConnection()
        {
            var conn = new SqlConnection(_connStr);
            conn.Open();
            conn.ChangeDatabase("FoolDB");
            return conn;
        }

        public async Task<bool> RegisterAsync(RegisterDTO dto)
        {
            using var conn = GetConnection();

            var exists = await conn.ExecuteScalarAsync<int>(
                "SELECT COUNT(*) FROM Players WHERE username = @Username",
                new { dto.Username }
            );

            if (exists > 0) { return false; }

            var hash = BCrypt.Net.BCrypt.HashPassword(dto.Password);

            await conn.ExecuteAsync(@"
                INSERT INTO Players
                    (username, password, display_name, wins, losses, total_score, games_played)
                VALUES
                    (@Username, @Password, @DisplayName, 0, 0, 0, 0)",
                new { dto.Username, Password = hash, dto.DisplayName }
            );

            return true;
        }
        public async Task<Player?> LoginAsync(LoginDTO dto)
        {
            using var conn = GetConnection();

            var player = await conn.QuerySingleOrDefaultAsync<Player>(
                "SELECT id AS Id," +
                "username AS Username," +
                "password AS Password," +
                "display_name AS DisplayName," +
                "wins AS Wins," +
                "losses AS Losses," +
                "total_score  AS TotalScore," +
                "games_played AS GamesPlayed" +
                " FROM Players WHERE username = @Username",
                new { dto.Username });

            if (player == null) { return null; }
            if (!BCrypt.Net.BCrypt.Verify(dto.Password, player.Password)) { return null; }

            return player;
        }
    }
}
