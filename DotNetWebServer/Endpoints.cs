using System.Net.Sockets;

namespace DotNetWebServer;

public static class Endpoints
{
    public static void MapEndpoints(this WebApplication app)
    {
        app.MapGet("/led", GetLedStatus);
        app.MapPost("/led/greenOn", TurnGreenLedOn);
        app.MapPost("/led/greenOff", TurnGreenLedOff);
        app.MapPost("/led/redOn", TurnRedLedOn);
        app.MapPost("/led/redOff", TurnRedLedOff);
    }

    public static LedStatus GetLedStatus()
    {
        return new LedStatus { GreenLed = true, RedLed = false };
    }

    public static IResult TurnGreenLedOn(HttpContext context)
    {
        Console.WriteLine("Green ON");
        return SendCommand(EspCommands.GreenLedOn);
    }

    public static IResult TurnGreenLedOff(HttpContext context)
    {
        Console.WriteLine("Green OFF");
        return SendCommand(EspCommands.GreenLedOff);
    }

    public static IResult TurnRedLedOn(HttpContext context)
    {
        Console.WriteLine("Red ON");
        return SendCommand(EspCommands.RedLedOn);
    }

    public static IResult TurnRedLedOff(HttpContext context)
    {
        Console.WriteLine("Red OFF");
        return SendCommand(EspCommands.RedLedOff);
    }

    private static IResult SendCommand(byte cmd)
    {
        var client = new TcpClient(Config.EspIpdAddr, Config.EspPort);

        try
        {
            var stream = client.GetStream();
            stream.WriteByte(cmd);
            var response = stream.ReadByte();
            client.Close();

            return response == EspResponse.Ok ? Results.Ok() : Results.StatusCode(500);
        }
        finally
        {
            client.Close();
        }
    }
}

public static class EspCommands
{
    public const byte RequestLedState = 1;
    public const byte GreenLedOn = 1;
    public const byte GreenLedOff = 2;
    public const byte RedLedOn = 3;
    public const byte RedLedOff = 4;
}

public static class EspResponse
{
    public const byte Ok = 0;
    public const byte Error = 1;
}

public static class Config
{
    public const string EspIpdAddr = "192.168.0.196";
    public const int EspPort = 80;
}

public class LedStatus
{
    public bool GreenLed { get; set; }
    public bool RedLed { get; set; }
}
