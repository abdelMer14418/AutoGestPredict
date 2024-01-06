from flask import Flask, render_template, request
import pickle
import numpy as np
import pandas as pd
from sklearn.preprocessing import StandardScaler
from sklearn.tree import DecisionTreeRegressor
from sklearn.model_selection import train_test_split
import pickle

# Spécifiez le chemin du fichier CSV
chemin_fichier_csv = 'C:/Users/macil/OneDrive/Bureau/Car/df.csv'


# Chargez le DataFrame depuis le fichier CSV
df_copie = pd.read_csv(chemin_fichier_csv)

scaler = StandardScaler()

numeric_columns = ['annee_du_modele', 'kilometrage', 'horsepower']
indexes = df_copie.index.tolist()
X_num = pd.DataFrame(scaler.fit_transform(df_copie[numeric_columns]), index = indexes, columns = numeric_columns)

binary_columns = ['accident', 'continent']
X_bin = pd.get_dummies(df_copie[binary_columns], columns = ['continent'], dtype = float)

X = pd.concat([X_num,X_bin], axis = 1)
y = df_copie['prix']



X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)


tree = DecisionTreeRegressor(max_depth = 5)
tree.fit(X_train, y_train)


# Sauvegardez le modèle
#with open('modele.pkl', 'wb') as fichier_modele:
#    pickle.dump(modele, fichier_modele)

app = Flask(__name__)




# Initialisation du scaler avec les colonnes numériques uniquement
numeric_columns_ = ['annee_du_modele', 'kilometrage', 'horsepower']
scaler_ = StandardScaler()
scaler_.fit(df_copie[numeric_columns_])

def preprocess_with_scaler(voiture, scaler_):
    # Définition des colonnes numériques
    numeric_columns_ = ['annee_du_modele', 'kilometrage', 'horsepower']

    # Prétraitement des caractéristiques numériques de la voiture avec le scaler
    voiture_num_ = pd.DataFrame(scaler_.transform(voiture[numeric_columns_]), columns=numeric_columns_)

    # Transformation des colonnes non numériques
    binary_columns_ = ['continent']
    voiture_non_num_ = voiture[binary_columns_].copy()
    voiture_bin_ = pd.get_dummies(voiture_non_num_, columns=['continent'], dtype=float)

    # Ajout de la colonne accident
    voiture_bin_['accident'] = voiture['accident']

    # Ajout des colonnes manquantes
    missing_cols_ = set(['continent_Amerique', 'continent_Asie', 'continent_Europe']) - set(voiture_bin_.columns)
    for c in missing_cols_:
        voiture_bin_[c] = 0.0

    # Concaténation des données numériques et binaires
    voiture_preprocessed = pd.concat([voiture_num_, voiture_bin_], axis=1)

    # Réorganisation des colonnes dans l'ordre souhaité
    ordered_cols_ = ['annee_du_modele', 'kilometrage', 'horsepower', 'accident', 'continent_Amerique', 'continent_Asie', 'continent_Europe']
    voiture_preprocessed = voiture_preprocessed.reindex(columns=ordered_cols_)

    return voiture_preprocessed
# Route for the home page
@app.route('/')
def man():
    pred = 0.0  # You can set any default value you prefer
    return render_template('home.html',pred=pred)

# Route for la prédiction
@app.route('/predict', methods=['POST'])
def predict():
    global tree

    # Récupération des données du formulaire
    data1 = int(request.form['a'])
    data2 = int(request.form['b'])
    data3 = float(request.form['c'])
    data4 = int(request.form['d'])
    data5 = request.form['e']

    # Création du DataFrame à partir des données du formulaire
    voiture = pd.DataFrame([{
        'annee_du_modele': data1,
        'kilometrage': data2,
        'horsepower': data3,
        'accident': data4,
        'continent': data5
    }])

    # Prétraitement des données de voiture
    voiture_scaler_processed = preprocess_with_scaler(voiture, scaler_)

    # Prédiction avec le modèle
    pred = tree.predict(voiture_scaler_processed)

    # Passer 'pred' à la template context
    return render_template('home.html', pred=pred)


if __name__ == "__main__":
    app.run(debug=True)
    
















