# Importing the required module
import matplotlib.pyplot as plt

# Given data
months = ['Jan', 'Feb', 'Mar']
water_before = [20, 18, 15]  # Simulating higher water usage in the summer
energy_before = [180, 160, 150]  # Simulating higher energy usage in the summer
chemicals_before = [3.5, 3, 2.5]  # Simulating higher chemical usage in the summer

water_after = [ 16, 14, 12]  # Reduction after IoT implementation
energy_after = [144, 128, 120]  # Reduction after IoT implementation
chemicals_after = [2.8, 2.4, 2]  # Reduction after IoT implementation

# Creating bar positions
bar_width = 0.35
index = range(len(months))
index_after = [i + bar_width for i in index]

# Plotting the bars for each category
plt.figure(figsize=(14, 6))

# Water Consumption
plt.subplot(1, 3, 1)
plt.bar(index, water_before, bar_width, label='Before', color='blue')
plt.bar(index_after, water_after, bar_width, label='After', color='cyan')
plt.xlabel('Months')
plt.ylabel('Cubic Meters')
plt.title('Water Consumption')
plt.xticks([r + bar_width / 2 for r in range(len(months))], months)
plt.legend()

# Energy Consumption
plt.subplot(1, 3, 2)
plt.bar(index, energy_before, bar_width, label='Before', color='orange')
plt.bar(index_after, energy_after, bar_width, label='After', color='yellow')
plt.xlabel('Months')
plt.ylabel('kWh')
plt.title('Energy Consumption')
plt.xticks([r + bar_width / 2 for r in range(len(months))], months)
plt.legend()

# Chemicals Consumption
plt.subplot(1, 3, 3)
plt.bar(index, chemicals_before, bar_width, label='Before', color='green')
plt.bar(index_after, chemicals_after, bar_width, label='After', color='lime')
plt.xlabel('Months')
plt.ylabel('Liters')
plt.title('Chemicals Consumption')
plt.xticks([r + bar_width / 2 for r in range(len(months))], months)
plt.legend()

# Show plot
plt.tight_layout()
plt.show()
