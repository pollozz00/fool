using Dapper;
using FoolWebAPI.DTOs;
using Microsoft.Data.SqlClient;
namespace FoolWebAPI.Services

{
    public class RatingService
    {
        private readonly string _connStr;

        public RatingService(string connStr) {
            _connStr = connStr;
        }

        public SqlConnection GetConnection() { 
            var conn = new SqlConnection(_connStr);
            conn.Open();
            conn.ChangeDatabase("FoolDB");
            return conn;
        }

        public async Task SaveResultAsync(SaveResultDTO dto)
        {
            using var conn = GetConnection();

            int absScore = Math.Abs(dto.Score);

            await conn.ExecuteAsync(@"
            INSERT INTO GameHistory (player_id, score, result, played_at)
            VALUES (@PlayerId, @Score, @Result, GETDATE())",
                    new { dto.PlayerId, Score = absScore, dto.Result }
                );

                if (dto.Result == "win")
                {
                    await conn.ExecuteAsync(@"
                    UPDATE Players SET
                        wins         = wins + 1,
                        total_score  = total_score + @Score,
                        games_played = games_played + 1
                    WHERE id = @PlayerId",
                            new { dto.PlayerId, Score = absScore }
                        );
                }
                else if (dto.Result == "loss")
                {
                    await conn.ExecuteAsync(@"
                    UPDATE Players SET
                        losses       = losses + 1,
                        total_score  = CASE 
                            WHEN total_score - @Score < 0 THEN 0 
                            ELSE total_score - @Score 
                        END,
                        games_played = games_played + 1
                    WHERE id = @PlayerId",
                            new { dto.PlayerId, Score = absScore }
                        );
                }
                else if (dto.Result == "draw")
                {
                    await conn.ExecuteAsync(@"
                    UPDATE Players SET
                        games_played = games_played + 1
                    WHERE id = @PlayerId",
                            new { dto.PlayerId }
                        );
                }
        }

        public async Task<IEnumerable<object>> GetLeaderboardAsync()
        {
            using var conn = GetConnection();
            return await conn.QueryAsync(@"
            SELECT
                username        AS Username,
                display_name    AS DisplayName,
                wins            AS Wins,
                losses          AS Losses,
                games_played    AS GamesPlayed,
                total_score     AS TotalScore
            FROM Players
            WHERE games_played > 0
            ORDER BY total_score DESC, wins DESC",
        
                    new { }
                );
        }
        public async Task<IEnumerable<object>> GetPlayerHistoryAsync(int playerId)
        {
            using var conn = GetConnection();
            return await conn.QueryAsync(@"
            SELECT
                id        AS Id,
                score     AS Score,
                result    AS Result,
                played_at AS PlayedAt
            FROM GameHistory
            WHERE player_id = @PlayerId
            ORDER BY played_at DESC",
                new { PlayerId = playerId }
            );
        }
    }
}

