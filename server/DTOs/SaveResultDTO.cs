namespace FoolWebAPI.DTOs
{
    public class SaveResultDTO
    {
        public int PlayerId { get; set; }
        public int Score { get; set; }
        public string Result { get; set; } = null!;
    }
}
