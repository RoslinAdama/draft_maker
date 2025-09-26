import time
from itertools import combinations
import pandas as pd


class DraftFromExcel:
    def __init__(
        self,
        excel_filepath: str,
        names_sheet: str = "Feuil1",
        grades_sheet: str = "Feuil2",
    ) -> None:
        self.team_names, self.teams_grades_data, self.teams_names_data = (
            self.load_team_data(excel_filepath, names_sheet, grades_sheet)
        )
        self.positions_to_fill = ["Ga", "Mdef", "Mc", "Moff", "Bu", "Arr", "Dc", "Ail"]
        self.single_player_roles = {"Ga", "Mdef", "Mc", "Moff", "Bu"}

    @staticmethod
    def load_team_data(excel_filepath: str, names_sheet: str, grades_sheet: str):
        team_names = []
        teams_grades_data = []
        teams_names_data = []

        try:
            names_df = pd.read_excel(excel_filepath, sheet_name=names_sheet)
            grades_df = pd.read_excel(excel_filepath, sheet_name=grades_sheet)

            names_df.fillna("N/A", inplace=True)
            grades_df.fillna(0, inplace=True)

            names_df.rename(columns={names_df.columns[0]: "Team"}, inplace=True)
            grades_df.rename(columns={grades_df.columns[0]: "Team"}, inplace=True)

            for index in range(len(names_df)):
                name_row = names_df.iloc[index]
                grade_row = grades_df.iloc[index]

                team_name = name_row["Team"]
                if team_name == "N/A":
                    continue

                team_names.append(team_name)

                def to_int(val):
                    return int(val)

                teams_grades_data.append(
                    {
                        "Ga": to_int(grade_row["G"]),
                        "Arr": [to_int(grade_row["Arr D"]), to_int(grade_row["Arr G"])],
                        "Dc": [to_int(grade_row["DCG"]), to_int(grade_row["DCD"])],
                        "Mdef": to_int(grade_row["MDEF"]),
                        "Mc": to_int(grade_row["MC"]),
                        "Moff": to_int(grade_row["MOFF"]),
                        "Ail": [to_int(grade_row["Ail G"]), to_int(grade_row["Ail D"])],
                        "Bu": to_int(grade_row["BU"]),
                    }
                )

                def to_str(val):
                    return str(val)

                teams_names_data.append(
                    {
                        "Ga": to_str(name_row["G"]),
                        "Arr": [to_str(name_row["Arr D"]), to_str(name_row["Arr G"])],
                        "Dc": [to_str(name_row["DCG"]), to_str(name_row["DCD"])],
                        "Mdef": to_str(name_row["MDEF"]),
                        "Mc": to_str(name_row["MC"]),
                        "Moff": to_str(name_row["MOFF"]),
                        "Ail": [to_str(name_row["Ail G"]), to_str(name_row["Ail D"])],
                        "Bu": to_str(name_row["BU"]),
                    }
                )

        except FileNotFoundError:
            print(
                f"fréro ton path excel est tellement cringe {excel_filepath}"
            )
            return None, None, None
        except ValueError as e:
            print(
                f"fréro tes noms de feuilles font la zoomba {names_sheet} et {grades_sheet} ça fait {e}"
            )
            return None, None, None
        except Exception as e:
            print(f"Clueless {e}")
            return None, None, None

        return team_names, teams_grades_data, teams_names_data

    def _optimistic_upper_bound(self, remaining_positions, remaining_teams):
        bound = 0
        for role in remaining_positions:
            all_grades_for_role = []
            for team_idx in remaining_teams:
                player_data = self.teams_grades_data[team_idx][role]
                if isinstance(player_data, list):
                    all_grades_for_role.extend(player_data)
                else:
                    all_grades_for_role.append(player_data)
            all_grades_for_role.sort(reverse=True)
            if role in self.single_player_roles:
                if all_grades_for_role:
                    bound += all_grades_for_role[0]
            else:
                if len(all_grades_for_role) >= 2:
                    bound += all_grades_for_role[0] + all_grades_for_role[1]
                elif all_grades_for_role:
                    bound += all_grades_for_role[0]
        return bound

    def _find_teams_hybrid(
        self,
        position_idx,
        current_composition,
        current_sum,
        available_teams,
        best_grade_so_far,
        all_compositions,
    ):
        remaining_positions = self.positions_to_fill[position_idx:]
        upper_bound = current_sum + self._optimistic_upper_bound(
            remaining_positions, available_teams
        )
        if upper_bound < best_grade_so_far[0]:
            return
        if position_idx == len(self.positions_to_fill):
            all_compositions.append(
                {"composition": list(current_composition), "total_grade": current_sum}
            )
            best_grade_so_far[0] = max(best_grade_so_far[0], current_sum)
            return
        position = self.positions_to_fill[position_idx]
        if position in self.single_player_roles:
            for team_idx in list(available_teams):
                grade = self.teams_grades_data[team_idx][position]
                if grade == 0:
                    continue
                available_teams.remove(team_idx)
                self._find_teams_hybrid(
                    position_idx + 1,
                    current_composition + [(team_idx, position, grade)],
                    current_sum + grade,
                    available_teams,
                    best_grade_so_far,
                    all_compositions,
                )
                available_teams.add(team_idx)
        else:
            for team1_idx, team2_idx in combinations(available_teams, 2):
                grades1, grades2 = (
                    self.teams_grades_data[team1_idx][position],
                    self.teams_grades_data[team2_idx][position],
                )
                new_available = available_teams - {team1_idx, team2_idx}
                for i, p1_grade in enumerate(grades1):
                    if p1_grade == 0:
                        continue
                    for j, p2_grade in enumerate(grades2):
                        if p2_grade == 0:
                            continue
                        new_comp = current_composition + [
                            (team1_idx, f"{position}_{i}", p1_grade),
                            (team2_idx, f"{position}_{j}", p2_grade),
                        ]
                        new_sum = current_sum + p1_grade + p2_grade
                        self._find_teams_hybrid(
                            position_idx + 1,
                            new_comp,
                            new_sum,
                            new_available,
                            best_grade_so_far,
                            all_compositions,
                        )

    def _format_composition(self, best_team_dict):
        player_list = []
        sorted_composition = sorted(best_team_dict["composition"], key=lambda x: x[1])
        for team_idx, pos_str, grade in sorted_composition:
            team = self.team_names[team_idx]
            if "_" in pos_str:
                role, player_idx_in_role = pos_str.split("_")
                player_name = self.teams_names_data[team_idx][role][
                    int(player_idx_in_role)
                ]
            else:
                role = pos_str
                player_name = self.teams_names_data[team_idx][role]
            player_list.append(
                {
                    "Position": pos_str,
                    "Team": team,
                    "Player": player_name,
                    "Grade": grade,
                }
            )
        return {"total_grade": best_team_dict["total_grade"], "players": player_list}

    def find_best_compo(self):
        if not self.team_names:
            return None
        all_found_compositions = []
        initial_available_teams = set(range(len(self.team_names)))
        best_grade_tracker = [-1]
        self._find_teams_hybrid(
            0,
            [],
            0,
            initial_available_teams,
            best_grade_tracker,
            all_found_compositions,
        )
        if not all_found_compositions:
            return None
        best_team = max(all_found_compositions, key=lambda x: x["total_grade"])
        return self._format_composition(best_team)


if __name__ == "__main__":
    EXCEL_FILE = "../data/exemple.xlsx"

    draft = DraftFromExcel(EXCEL_FILE)

    if draft.team_names:
        start_time = time.time()
        best_composition = draft.find_best_compo()
        duration = time.time() - start_time

        print(f"{duration:.4f} second")

        if best_composition:
            print("-" * 50)
            print(
                f"\n best team bitches (Total Grade: {best_composition['total_grade']})"
            )
            print("-" * 50)

            for player in best_composition["players"]:
                print(
                    f"  - Pos: {player['Position']:<7} | Team: {player['Team']:<15} | Player: {player['Player']:<20} | Grade: {player['Grade']}"
                )
            print("-" * 50)
