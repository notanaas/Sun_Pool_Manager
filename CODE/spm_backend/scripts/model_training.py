# model_training.py
import pandas as pd
from joblib import dump
from sklearn.ensemble import RandomForestClassifier
from data_preparation import load_data, preprocess_data

def train_model():
    df = load_data()
    df_prepared = preprocess_data(df)

    X = df_prepared.drop(['action_taken', 'action_result'], axis=1)
    y = df_prepared['action_taken']

    clf = RandomForestClassifier()
    clf.fit(X, y)

    # Save the trained model
    dump(clf, 'models/model.joblib')

if __name__ == '__main__':
    train_model()
