import random as rd
import time
from itertools import combinations

num_teams = 15
teams_data = []
for i in range(num_teams):
    team = {
        "Ga": rd.randint(1, 10),
        "Arr": [rd.randint(1, 10), rd.randint(1, 10)],
        "Dc": [rd.randint(1, 10), rd.randint(1, 10)],
        "Mdef": rd.randint(1, 10),
        "Mc": rd.randint(1, 10),
        "Moff": rd.randint(1, 10),
        "Ail": [rd.randint(1, 10), rd.randint(1, 10)],
        "Bu": rd.randint(1, 10),
    }
    teams_data.append(team)

positions_to_fill = ["Ga", "Mdef", "Mc", "Moff", "Bu", "Arr", "Dc", "Ail"]
single_player_roles = {"Ga", "Mdef", "Mc", "Moff", "Bu"}


def optimistic_upper_bound(remaining_positions, remaining_teams):
    """
    Calculates the best possible score from the current state, ignoring the
    "one player per team" constraint to create an optimistic (but valid) upper bound.
    """
    bound = 0
    for role in remaining_positions:
        all_grades_for_role = []
        for team_idx in remaining_teams:
            player_data = teams_data[team_idx][role]
            if isinstance(player_data, list):
                all_grades_for_role.extend(player_data)
            else:
                all_grades_for_role.append(player_data)

        all_grades_for_role.sort(reverse=True)

        if role in single_player_roles:
            if all_grades_for_role:
                bound += all_grades_for_role[0]
        else: 
            if len(all_grades_for_role) >= 2:
                bound += all_grades_for_role[0] + all_grades_for_role[1]
            elif all_grades_for_role:  
                bound += all_grades_for_role[0]

    return bound



all_valid_compositions = []


def find_teams_hybrid(
    position_idx, current_composition, current_sum, available_teams, best_grade_so_far
):
    remaining_positions = positions_to_fill[position_idx:]
    upper_bound = current_sum + optimistic_upper_bound(
        remaining_positions, available_teams
    )
    if upper_bound < best_grade_so_far[0]:
        return

    if position_idx == len(positions_to_fill):
        all_valid_compositions.append(
            {"composition": list(current_composition), "total_grade": current_sum}
        )
        best_grade_so_far[0] = max(best_grade_so_far[0], current_sum)
        return

    position = positions_to_fill[position_idx]

    if position in single_player_roles:
        for team_idx in list(available_teams):
            grade = teams_data[team_idx][position]
            available_teams.remove(team_idx)
            find_teams_hybrid(
                position_idx + 1,
                current_composition + [(team_idx, position, grade)],
                current_sum + grade,
                available_teams,
                best_grade_so_far,
            )
            available_teams.add(team_idx)  

    else:
        for team1_idx, team2_idx in combinations(available_teams, 2):
            grades1, grades2 = (
                teams_data[team1_idx][position],
                teams_data[team2_idx][position],
            )
            new_available = available_teams - {team1_idx, team2_idx}
            for p1_grade in grades1:
                for p2_grade in grades2:
                    new_comp = current_composition + [
                        (team1_idx, f"{position}_0", p1_grade),
                        (team2_idx, f"{position}_1", p2_grade),
                    ]
                    new_sum = current_sum + p1_grade + p2_grade
                    find_teams_hybrid(
                        position_idx + 1,
                        new_comp,
                        new_sum,
                        new_available,
                        best_grade_so_far,
                    )


print("start")
start_time = time.time()

initial_available_teams = set(range(num_teams))
best_grade_tracker = [-1]

find_teams_hybrid(0, [], 0, initial_available_teams, best_grade_tracker)

end_time = time.time()
duration = end_time - start_time

print(f"len(all_valid_compositions)")
print(f" {duration:.4f} second")

if all_valid_compositions:
    best_team = max(all_valid_compositions, key=lambda x: x["total_grade"])
    print(f"\nteam grade: {best_team['total_grade']}")
    for team_idx, pos, grade in sorted(best_team["composition"]):
        print(f"  - Position: {pos:<7} | From Team: {team_idx:<2} | Grade: {grade}")