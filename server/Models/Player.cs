namespace FoolWebAPI.Models
{
    public class Player
    {
        public int Id { get; set; }
        public string Username { get; set; } = null!;

        public string Password { get; set; } = null!;

        public string DisplayName { get; set; } = null!;

        public int Wins { get; set; }
        public int Losses { get; set; }
        public int TotalScore { get; set; }
        public int GamesPlayed { get; set; }


    }
}
