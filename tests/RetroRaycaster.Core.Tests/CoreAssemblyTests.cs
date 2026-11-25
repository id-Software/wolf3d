using System.Reflection;
using Xunit;

namespace RetroRaycaster.Core.Tests;

public class CoreAssemblyTests
{
    [Fact]
    public void CoreLoadsAsLibrary()
    {
        var core = Assembly.Load("RetroRaycaster.Core");

        Assert.Null(core.EntryPoint);
    }
}
