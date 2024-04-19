from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime, timedelta
from apscheduler.schedulers.background import BackgroundScheduler
import os

from config import DevelopmentConfig, ProductionConfig

app = Flask(__name__)
app.config.from_object(DevelopmentConfig())  # Switch to ProductionConfig as needed
db = SQLAlchemy(app)

class SensorData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    water_level = db.Column(db.Float, nullable=False)
    ph_value = db.Column(db.Float, nullable=False)
    voltage = db.Column(db.Float, nullable=False)
    current = db.Column(db.Float, nullable=False)
    motion_detected = db.Column(db.Float, nullable=False)
    water_temperature = db.Column(db.Float, nullable=False)
    action_taken = db.Column(db.String(250))  # Extended length for multiple actions
    action_result = db.Column(db.String(50))  # Outcome of the action
    timestamp = db.Column(db.DateTime, default=datetime.utcnow, nullable=False)

@app.before_request
def initialize_database():
    db.create_all()
    schedule_filtration_tasks()

def schedule_filtration_tasks():
    scheduler = BackgroundScheduler()
    scheduler.add_job(func=trigger_filtration, trigger="cron", hour="8,20")
    scheduler.start()

def trigger_filtration():
    # Logic to trigger filtration here
    print("Filtration system activated.")

@app.route('/sensor_data_post', methods=['POST'])
def receive_data():
    data = request.get_json()
    required_fields = ['water_level', 'ph_value', 'voltage', 'current', 'motion_detected', 'water_temperature']
    if not all(field in data for field in required_fields):
        missing = [field for field in required_fields if field not in data]
        return jsonify({"error": f"Missing data for fields: {', '.join(missing)}"}), 400

    for field in required_fields:
        if data[field] < 0:
            return jsonify({"error": f"Invalid value for {field}, must be positive"}), 400

    action_details = determine_actions(data)
    data.update(action_details)

    new_data = SensorData(**data)
    db.session.add(new_data)
    db.session.commit()
    return jsonify({"message": "Data logged successfully", "actions": action_details}), 201

def determine_actions(data):
    actions = {}
    if data['water_temperature'] < 20:
        actions['action_taken'] = 'Heater turned on; '
    if data['ph_value'] > 7.5:
        actions['action_taken'] = actions.get('action_taken', '') + 'Chlorine motor turned on; '
    if data['water_level'] < 10:
        actions['action_taken'] = actions.get('action_taken', '') + 'Water pump turned on to fill tank; '
    elif data['water_level'] > 80:
        actions['action_taken'] = actions.get('action_taken', '') + 'Water pump turned on to drain tank; '
    if data['current'] < 5 and data['voltage'] < 110:  # Example thresholds for maintenance
        actions['action_taken'] = actions.get('action_taken', '') + 'Maintenance needed for solar panels; '
    actions['action_result'] = 'Success'  # Simplified assumption
    return actions

@app.route('/data', methods=['GET'])
def send_data():
    data = SensorData.query.order_by(SensorData.id.desc()).first()
    if data:
        return jsonify({col.name: getattr(data, col.name) for col in data.__table__.columns}), 200
    return jsonify({}), 204
ESP_IP_ADDRESS = "192.168.1.29"

@app.route('/command', methods=['POST'])
def handle_command():
    command = request.json.get('command')
    if command == 'turn_on_motor':
        response = requests.get(f'http://{ESP_IP_ADDRESS}/motor_on')
        if response.status_code == 200:
            print("Motor turned on")
            return jsonify({'message': 'Motor turned on successfully'}), 200
        else:
            return jsonify({'error': 'Failed to send command to ESP'}), 500
    elif command == 'turn_off_motor':
        response = requests.get(f'http://{ESP_IP_ADDRESS}/motor_off')
        if response.status_code == 200:
            print("Motor turned off")
            return jsonify({'message': 'Motor turned off successfully'}), 200
        else:
            return jsonify({'error': 'Failed to send command to ESP'}), 500

    return jsonify({'error': 'Invalid command'}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)

