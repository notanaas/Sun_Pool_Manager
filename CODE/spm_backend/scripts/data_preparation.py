# data_preparation.py
import pandas as pd
from sqlalchemy import create_engine
from sklearn.preprocessing import StandardScaler

def load_data():
    engine = create_engine('sqlite:///sensor_data.db')
    df = pd.read_sql_table('sensor_data', engine)
    return df

def preprocess_data(df):
    df.fillna(df.mean(), inplace=True)
    for col in ['water_level', 'ph_value', 'voltage', 'current', 'water_temperature']:
        q_low = df[col].quantile(0.01)
        q_hi = df[col].quantile(0.99)
        df = df[(df[col] > q_low) & (df[col] < q_hi)]

    df['timestamp'] = pd.to_datetime(df['timestamp'])
    df['hour'] = df['timestamp'].dt.hour
    df['day_of_week'] = df['timestamp'].dt.dayofweek

    scaler = StandardScaler()
    df[['water_level', 'ph_value', 'voltage', 'current', 'water_temperature']] = scaler.fit_transform(df[['water_level', 'ph_value', 'voltage', 'current', 'water_temperature']])
    return df
