namespace FoolWebAPI.Models
{
    public class GameHistory
    {
        public int Id { get; set; } = 0!;
        public int PlayerId { get; set; } = 0!;
        public int Score { get; set; }
        public string Result { get; set; } = null!;
        public DateTime PlayedAt { get; set; } 

    }
}
