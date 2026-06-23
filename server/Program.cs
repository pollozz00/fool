
using FoolWebAPI.Data;
using FoolWebAPI.Services;
using System.Text.Json;

namespace FoolWebAPI
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var builder = WebApplication.CreateBuilder(args);

            // Add services to the container.
            string connStr = builder.Configuration.GetConnectionString("DefaultConnection")!;

            builder.Services.AddControllers();

            // Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
            builder.Services.AddEndpointsApiExplorer();
            builder.Services.AddSwaggerGen();

            builder.Services.AddScoped<AuthService>(_ => new AuthService(connStr));
            builder.Services.AddScoped<RatingService>(_ => new RatingService(connStr));

            var app = builder.Build();

            new DatabaseInit(connStr).Init();
            // Configure the HTTP request pipeline.
            if (app.Environment.IsDevelopment())
            {
                app.UseSwagger();
                app.UseSwaggerUI();
            }
            

            app.UseHttpsRedirection();

            app.UseAuthorization();

            app.UseDefaultFiles();
            app.UseStaticFiles();

            app.MapControllers();

            app.Run();
        }
    }
}
