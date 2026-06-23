using FoolWebAPI.DTOs;
using FoolWebAPI.Services;
using Microsoft.AspNetCore.Http.HttpResults;
using Microsoft.AspNetCore.Mvc;

namespace FoolWebAPI.Controllers
{
    [Route("api/Rating")]
    [ApiController]
    public class RatingController : ControllerBase
    {
        private readonly RatingService _ratingService;

        public RatingController(RatingService ratingService)
        {
            _ratingService = ratingService;
        }

        [HttpPost("save")]
        public async Task<IActionResult> SaveResult([FromBody] SaveResultDTO dto)
        {
            if (dto.PlayerId <= 0)
                return BadRequest("Невірний PlayerId");

            if (dto.Result != "win" && dto.Result != "loss" && dto.Result != "draw")
                return BadRequest("Result має бути: win, loss або draw");

            await _ratingService.SaveResultAsync(dto);
            return Ok(new { message = "Результат збережено!" });
        }

        [HttpGet("leaderboard")]
        public async Task<IActionResult> GetLeaderboard()
        {
            var players = await _ratingService.GetLeaderboardAsync();
            return Ok(players);
        }

        [HttpGet("history/{playerId}")]
        public async Task<IActionResult> GetHistory(int playerId)
        {
            var history = await _ratingService.GetPlayerHistoryAsync(playerId);
            return Ok(history);
        }
    }
}
