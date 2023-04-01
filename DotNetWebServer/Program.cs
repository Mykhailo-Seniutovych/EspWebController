using DotNetWebServer;

var builder = WebApplication.CreateBuilder(args);

builder.WebHost.ConfigureKestrel(serverOptions =>
{
    serverOptions.ListenAnyIP(5000);
});

var app = builder.Build();

app.MapEndpoints();

app.UseStaticFiles();

app.Run();
