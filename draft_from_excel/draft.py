import pandas as pd
from typing import Tuple, List
from random import choice
from tqdm import tqdm
import numpy as np

N_COMPOS_TO_GENERATE = 50000

class DraftFromExcel:
    
    df_noms : pd.DataFrame
    df_notes : pd.DataFrame
    joueurs : dict


    def __init__(self, filepath : str) -> None:
        
        self.df_noms, self.df_notes = DraftFromExcel.load_excel(filepath)

    @staticmethod
    def load_excel(path : str) -> Tuple[pd.DataFrame, pd.DataFrame]:

        df_noms = pd.read_excel(path, sheet_name=0)
        df_notes = pd.read_excel(path, sheet_name=1)

        return df_noms, df_notes
    

##############################################################################
    
    def get_joueurs(self):

        joueurs = {}
        for column in self.df_noms.columns:
            joueurs[column] = self.df_noms[column].dropna().to_list()

        return joueurs
    

    def generate_random_compos(self, n_compos_to_generate = N_COMPOS_TO_GENERATE):

        if "Unnamed: 0" in self.joueurs.keys():
            del self.joueurs["Unnamed: 0"]

        compos_random = []

        for i in tqdm(range(n_compos_to_generate)):
            compo = []
            for j_potentiels in self.joueurs.values():
                joueur_choisi = choice(j_potentiels)
                compo.append(joueur_choisi)

            if compo not in compos_random:
                compos_random.append(compo)

        return compos_random
    

    def find_equipe(self, joueur : str):

        index_equipe = self.df_noms[self.df_noms.values == joueur].index[0]
        return self.df_noms.at[index_equipe, "Unnamed: 0"]
    

    def filter_possible_compos(self, initial_compos : list) -> list:
        compos_valides = []
        
        for compo in tqdm(initial_compos): 
            equipes = [self.find_equipe(joueur) for joueur in compo]

            if not any([equipes.count(value)>1 for value in equipes]):
                compos_valides.append(compo)

        return compos_valides
    

##############################################################################
    
    def notes_from_nom(self, nom_joueur : str) -> float:

        result_search = np.where(self.df_noms == nom_joueur)
        num_ligne = result_search[0][0]
        num_col = result_search[1][0]

        return self.df_notes.iloc[num_ligne, num_col]
    
    
    def valeur_equipe(self, equipe : List[str]) -> float:

        valeurs = [self.notes_from_nom(joueur) for joueur in equipe]

        return sum(valeurs)
    
##############################################################################
    

    def find_best_compo(self):

        self.joueurs = self.get_joueurs()

        compos_random = self.generate_random_compos()
        compos_valides = self.filter_possible_compos(initial_compos=compos_random)

        valeurs_equipes = [ self.valeur_equipe(compo) for compo in compos_valides ]

        meilleure_compo = compos_valides[valeurs_equipes.index(max(valeurs_equipes))]

        return meilleure_compo
