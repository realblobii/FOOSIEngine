import json

# Size of the grid
GRID_WIDTH = 75
GRID_HEIGHT = 75

tiles = []

# Generate grass tiles for z = 0
for y in range(GRID_HEIGHT):
    for x in range(GRID_WIDTH):
        tiles.append({
            "obj_subclass": "grass",
            "x": x,
            "y": y,
            "z": 0
        })

# Build final dictionary
grid_data = {
    "tiles": tiles
}

# Write to a JSON file
with open("grass_grid.json", "w") as f:
    json.dump(grid_data, f, indent=2)

print("Generated grass_grid.json with 50x50 grass tiles.")
