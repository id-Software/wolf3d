using System;
using Xunit;

namespace RetroRaycaster.Core.Tests;

public class GridMapTests
{
    [Theory]
    [InlineData(1.5, 2.5, 1, 0, 4, 2, WallSide.Vertical, 2.5)]
    [InlineData(1.5, 2.5, 2, 0, 4, 2, WallSide.Vertical, 1.25)]
    [InlineData(4.5, 2.5, -1, 0, 1, 2, WallSide.Vertical, 2.5)]
    [InlineData(3.5, 0.5, 0, 1, 3, 2, WallSide.Horizontal, 1.5)]
    [InlineData(3.5, 2.5, 0, -1, 3, 0, WallSide.Horizontal, 1.5)]
    [InlineData(0.75, 0.25, 2, 1, 2, 1, WallSide.Horizontal, 0.75)]
    [InlineData(3, 1, -1, 0, 2, 1, WallSide.Vertical, 0)]
    public void HitsFirstWall(double x, double y, double dx, double dy,
        int wallX, int wallY, WallSide side, double t)
    {
        var cells = new bool[3, 6];
        cells[wallY, wallX] = true;
        cells[2, 5] = true;
        Assert.Equal(new RayHit(wallX, wallY, side, t), new GridMap(cells).CastRay(x, y, dx, dy));
    }

    [Fact]
    public void CornerCrossingSkipsWallsTouchedOnlyAtTheirVertex()
    {
        var cells = new bool[,] { { false, true }, { true, false } };
        Assert.Null(new GridMap(cells).CastRay(0.5, 0.5, 1, 1));

        cells[1, 1] = true;
        Assert.Equal(new RayHit(1, 1, WallSide.Vertical, 0.5), new GridMap(cells).CastRay(0.5, 0.5, 1, 1));
    }

    [Theory]
    [InlineData(0.5, 0.5, 1, 0)]
    [InlineData(0.5, 0.5, 0, 0)]
    [InlineData(-0.5, 1.5, 1, 0)]
    [InlineData(4, 1.5, -1, 0)]
    [InlineData(2.5, 2, 0, -1)]
    public void ReturnsNoHit(double x, double y, double dx, double dy)
    {
        var cells = new bool[2, 4];
        cells[1, 2] = true;
        Assert.Null(new GridMap(cells).CastRay(x, y, dx, dy));
    }

    [Fact]
    public void RejectsMovingRayStartingInsideWall()
    {
        var map = new GridMap(new bool[,] { { true } });
        Assert.Throws<ArgumentException>(() => map.CastRay(0.5, 0.5, 1, 0));
    }
}
