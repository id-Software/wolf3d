using System.Reflection;
using System.Runtime.Versioning;
using Xunit;

namespace RetroRaycaster.Core.Tests;

public class CoreAssemblyTests
{
    [Fact]
    public void CoreAssemblyTargetsNet8()
    {
        var core = Assembly.Load("RetroRaycaster.Core");
        var target = core.GetCustomAttribute<TargetFrameworkAttribute>();

        Assert.Equal(".NETCoreApp,Version=v8.0", target?.FrameworkName);
    }
}
