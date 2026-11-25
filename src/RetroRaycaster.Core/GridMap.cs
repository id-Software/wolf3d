using System;

namespace RetroRaycaster.Core;

public enum WallSide { Vertical, Horizontal }

/// <summary>The entry point into the wall is origin + direction * T.</summary>
public readonly record struct RayHit(int CellX, int CellY, WallSide Side, double T);

/// <summary>A grid of unit cells. X increases rightward and Y downward.</summary>
public sealed class GridMap
{
    private readonly bool[,] solid;
    public int Width => solid.GetLength(1);
    public int Height => solid.GetLength(0);

    /// <param name="solidCells">Rows indexed by [y, x]. True denotes a wall.</param>
    public GridMap(bool[,] solidCells)
    {
        ArgumentNullException.ThrowIfNull(solidCells);
        if (solidCells.GetLength(0) == 0 || solidCells.GetLength(1) == 0)
            throw new ArgumentException("The grid must contain at least one row and column.", nameof(solidCells));

        solid = (bool[,])solidCells.Clone();
    }

    /// <summary>
    /// Finds the first wall entered, or returns null for an outside origin, a zero
    /// direction, or a ray that leaves the map. Directions need not have unit length.
    /// The origin belongs to cell (floor(x), floor(y)) and must be empty if inside.
    /// At a corner the ray enters only the diagonal cell and reports its vertical side.
    /// </summary>
    public RayHit? CastRay(double originX, double originY, double directionX, double directionY)
    {
        if (!double.IsFinite(originX) || !double.IsFinite(originY) ||
            !double.IsFinite(directionX) || !double.IsFinite(directionY))
            throw new ArgumentException("Ray coordinates and direction must be finite.");

        if (originX < 0 || originX >= Width || originY < 0 || originY >= Height ||
            (directionX == 0 && directionY == 0))
            return null;

        int x = (int)originX;
        int y = (int)originY;
        if (solid[y, x])
            throw new ArgumentException("A moving ray must start in an empty cell.");

        int stepX = Math.Sign(directionX);
        int stepY = Math.Sign(directionY);
        double nextX = CrossingTime(x, originX, directionX);
        double nextY = CrossingTime(y, originY, directionY);

        while (true)
        {
            bool crossX = stepX != 0 && (stepY == 0 || nextX <= nextY);
            bool crossY = stepY != 0 && (stepX == 0 || nextY <= nextX);
            double t = crossX ? nextX : nextY;

            if (crossX) x += stepX;
            if (crossY) y += stepY;
            if (x < 0 || x >= Width || y < 0 || y >= Height)
                return null;
            if (solid[y, x])
                return new RayHit(x, y, crossX ? WallSide.Vertical : WallSide.Horizontal, t);

            // Compute from the original ray to avoid accumulating rounding at corners.
            if (crossX) nextX = CrossingTime(x, originX, directionX);
            if (crossY) nextY = CrossingTime(y, originY, directionY);
        }
    }

    private static double CrossingTime(int cell, double origin, double direction)
    {
        if (direction == 0) return double.PositiveInfinity;
        int boundary = direction > 0 ? cell + 1 : cell;
        return (boundary - origin) / direction;
    }
}
