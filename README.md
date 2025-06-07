# 🚦 Traffic Signal Algorithm

## Overview

The Traffic Signal Algorithm is a C program that simulates intelligent traffic light management at a four-way intersection (North, East, South, and West). It prioritizes emergency vehicles, ensures fair flow for regular traffic, and logs processed vehicles for reference.

## 💡 Features

- Vehicle Management: Add vehicles with a unique ID, priority (regular/emergency), and direction (N, E, S, W).
- Queue Management: Maintains separate queues for each lane using linked lists.
- Emergency Vehicle Priority: Emergency vehicles are always given precedence using dynamic prioritization.
- Partial Regular Flow: Ensures 70% regular vehicles move when others are waiting, preventing lane starvation.
- Traffic Log: Logs every processed vehicle for post-simulation review.
- Robust Input Handling: Detects invalid input types, duplicate vehicle IDs, and enforces constraints.
- Interactive Menu: Allows users to:
  - Create vehicle entries
  - Display lane queues
  - Manage traffic flow
  - View the traffic log

## Data Structures Used

- 🚗 **Vehicle**  
  Holds vehicle-specific data like:
  - `id`: Unique 3-digit identifier  
  - `priority`: `0` for regular, `1` for emergency  
  - `direction`: One of `"N"`, `"E"`, `"S"`, `"W"`  
  - `next`: Pointer to the next vehicle (linked list)

-  **Queue**  
  Used to maintain lane-wise order of vehicles:
  - `front`: Points to the first vehicle in the lane  
  - `rear`: Points to the last vehicle  
  - Implemented using a singly linked list

- 📝 **Log**  
  Keeps a log of all processed vehicles:
  - `id`, `priority`, and `direction` like in `Vehicle`  
  - `next`: Pointer to the next log entry


## 🧠 Core Algorithm

1. Emergency Vehicles First: 
   - All emergency vehicles are processed in lane order of priority.
   - Lane priority is dynamically determined based on emergency count.

2. Regular Vehicles: 
   - Up to 70% of regular vehicles in a lane are allowed to proceed if other lanes are occupied.

3. Final Clearance:
   - Once emergency vehicles are cleared, the system allows all remaining vehicles to proceed.

## ⚙️ Menu Flow

```
MENU:
1. Create Traffic
2. Display All Vehicles
3. Manage Traffic
4. Traffic Log
Press $ to exit.
```

## Input Constraints

- Vehicle ID: Must be a 3-digit number (100–999)
- Priority: 0 for regular, 1 for emergency
- Direction: One of N, E, S, W
- No Duplicates: Duplicate vehicle IDs are rejected

## Sample Run

```bash
Enter Vehicle ID (3-digit), Priority (0 or 1), and Direction (N, E, S, W) or '$' to stop:
101 0 N
102 1 E
103 0 S
$
```

## 🗃 File Structure

```
├── CBP.c                   # Main C program with full traffic signal logic
├── README.md               # Documentation
├── DS CBP Documentation.pdf
├── Traffic Signal Algorithm.docx
```

## 👨‍💻 How to Compile and Run

```bash
gcc CBP.c -o traffic
./traffic
```

## 📝 Development 

- Implemented in C using K&R style
- Developed as a part of a data structures project simulating real-world intersections

## ✅ Future Enhancements

- Real-time visualization of lane status
- Integration with hardware (e.g., Arduino traffic lights)
- Priority classes (e.g., fire truck > ambulance > police > regular)
