import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Sensor Dashboard',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: SensorPage(),
    );
  }
}

class SensorPage extends StatefulWidget {
  @override
  _SensorPageState createState() => _SensorPageState();
}

class _SensorPageState extends State<SensorPage> {
  List<String> sensorValues = List.filled(6, "0");
  Timer? timer;
  final List<String> sensorNames = [
    "Water Level",
    "pH Value",
    "Voltage",
    "Current",
    "Motion Detected",
    "Water Temperature"
  ];
  final List<String> sensorUnits = [
    "cm", // Unit for Water Level
    "pH", // Unit for pH Value
    "V", // Unit for Voltage
    "A", // Unit for Current
    "", // No unit for Motion Detected (assuming it's binary)
    "°C", // Unit for Water Temperature
  ];
  final String backendUrl = "http://127.0.0.1:5000/data";

  @override
  void initState() {
    super.initState();
    timer = Timer.periodic(Duration(seconds: 2), (Timer t) => fetchData());
  }

  Future<void> fetchData() async {
    try {
      final response = await http.get(Uri.parse(backendUrl));
      if (response.statusCode == 200) {
        final data = json.decode(response.body);
        setState(() {
          // Set the values without units
          sensorValues[0] = data['water_level'].toString();
          sensorValues[1] = data['ph_value'].toString();
          sensorValues[2] = data['voltage'].toString();
          sensorValues[3] = data['current'].toString();
          sensorValues[4] = data['motion_detected'].toString();
          sensorValues[5] = data['water_temperature'].toString();
        });
      } else {
        throw Exception('Failed to load sensor data');
      }
    } catch (e) {
      print('Caught error: $e');
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text("Failed to fetch data from server")),
      );
    }
  }

  Future<void> sendCommand(String command) async {
    try {
      final response = await http.post(
        Uri.parse("http://127.0.0.1:5000/command"),
        headers: {'Content-Type': 'application/json'},
        body: json.encode({'command': command}),
      );
      if (response.statusCode == 200) {
        print("Command sent successfully");
      } else {
        throw Exception('Failed to send command');
      }
    } catch (e) {
      print('Caught error: $e');
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text("Failed to send command to server")),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Sensor Dashboard'),
        centerTitle: true,
      ),
      body: ListView.builder(
        padding: const EdgeInsets.all(20.0),
        itemCount: sensorNames.length + 1, // Adding one for the extra tile
        itemBuilder: (context, index) {
          if (index < sensorNames.length) {
            String unit = sensorUnits[index];
            String value = sensorValues[index];
            VoidCallback? action;
            IconData icon = Icons.sensor_door; // Default icon

            switch (index) {
              case 0:
                action = () => showModalWaterPumpControls(context);
                icon = Icons.water;
                break;
              case 1:
                action = () => sendCommand('toggle_chlorine_pump');
                icon = Icons.opacity;
                break;
              case 5:
                action = () => sendCommand('toggle_heater');
                icon = Icons.thermostat;
                break;
              case 2: // Voltage - No action
              case 3: // Current - No action
                icon = Icons.electrical_services;
                break;
              case 4: // Motion Detected
                icon = Icons.motion_photos_on;
                break;
            }

            return SensorDataTile(
              icon: icon,
              label: sensorNames[index],
              value: sensorValues[index],
              unit: sensorUnits[index],
              triggerAction: action,
            );
          } else {
            // The last tile for Motor Control
            return SensorDataTile(
              icon: Icons.power_settings_new,
              label: "Motor Control",
              value: "Tap to toggle",
              unit: "", // No unit for this control
              triggerAction: () => showModalMotorControls(context),
            );
          }
        },
      ),
    );
  }

// ... other code ...

  void showModalMotorControls(BuildContext context) {
    showModalBottomSheet(
      context: context,
      builder: (BuildContext context) {
        return Container(
          padding: EdgeInsets.all(20),
          child: Wrap(
            children: <Widget>[
              ListTile(
                  leading: Icon(Icons.power),
                  title: Text('Turn On Motor'),
                  onTap: () {
                    sendCommand('turn_on_motor');
                    Navigator.pop(context);
                  }),
              ListTile(
                  leading: Icon(Icons.power_off),
                  title: Text('Turn Off Motor'),
                  onTap: () {
                    sendCommand('turn_off_motor');
                    Navigator.pop(context);
                  }),
            ],
          ),
        );
      },
    );
  }

  void showModalWaterPumpControls(BuildContext context) {
    showModalBottomSheet(
      context: context,
      builder: (BuildContext context) {
        return Container(
          padding: EdgeInsets.all(20),
          child: Wrap(
            children: <Widget>[
              ListTile(
                  leading: Icon(Icons.water_drop),
                  title: Text('Fill Water Tank'),
                  onTap: () {
                    sendCommand('fill_water_tank');
                    Navigator.pop(context);
                  }),
              ListTile(
                  leading: Icon(Icons.water_damage), // Changed icon here
                  title: Text('Drain Water Tank'),
                  onTap: () {
                    sendCommand('drain_water_tank');
                    Navigator.pop(context);
                  }),
            ],
          ),
        );
      },
    );
  }

  @override
  void dispose() {
    timer?.cancel();
    super.dispose();
  }
}

class SensorDataTile extends StatelessWidget {
  final IconData icon;
  final String label;
  final String value;
  final String unit; // Add unit parameter
  final VoidCallback? triggerAction;

  const SensorDataTile({
    required this.icon,
    required this.label,
    required this.value,
    required this.unit, // Ensure this parameter is always provided
    this.triggerAction,
  });

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 2.0,
      child: ListTile(
        leading: Icon(icon, color: Colors.blue),
        title: Text(label),
        subtitle: Text('$value $unit'), // Display value with unit
        trailing: triggerAction != null
            ? IconButton(
                icon: Icon(Icons.touch_app),
                onPressed: triggerAction,
              )
            : null, // Only display the button if an action is provided
      ),
    );
  }
}
