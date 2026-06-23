using FoolWebAPI.DTOs;
using FoolWebAPI.Services;
using Microsoft.AspNetCore.Mvc;

namespace FoolWebAPI.Controllers
{
    [Route("api/Player")]
    [ApiController]
    public class PlayerController : ControllerBase
    {
        private readonly AuthService _authService;

        public PlayerController(AuthService authService)
        {
            _authService = authService;
        }

        [HttpPost("register")]
        public async Task<IActionResult> Register([FromBody] RegisterDTO dto)
        {
            if (string.IsNullOrWhiteSpace(dto.Username) ||
                string.IsNullOrWhiteSpace(dto.Password) ||
                string.IsNullOrWhiteSpace(dto.DisplayName))
                return BadRequest("Заповни всі поля");

            var success = await _authService.RegisterAsync(dto);

            if (!success)
                return Conflict("Username вже зайнятий");

            return Created("", new { message = "Аккаунт створено!" });
        }

        [HttpPost("login")]
        public async Task<IActionResult> Login([FromBody] LoginDTO dto)
        {
            var player = await _authService.LoginAsync(dto);

            if (player == null)
                return Unauthorized("Невірний логін або пароль");

            return Ok(new
            {
                player.Id,
                player.Username,
                player.DisplayName,
                player.Wins,
                player.Losses,
                player.TotalScore,
                player.GamesPlayed
            });
        }
    }
}