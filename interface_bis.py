import streamlit as st
from draft_from_excel.draft_bis import DraftFromExcel

st.set_page_config(layout="wide")


if __name__ == "__main__":

    st.title("Draft maker")

    input_filepath = st.text_input(label="Path to excel file", value="data/exemple.xlsx")

    if st.button("Lancer recherche meilleure compo"):

        draft_maker = DraftFromExcel(input_filepath)

        with st.spinner(text="Finding best compos... (can tale a few minutes)"):
            meilleure_compo = draft_maker.find_best_compo()

            st.session_state["meilleure_compo"] = meilleure_compo
            st.rerun()

    if "meilleure_compo" in st.session_state.keys():
        st.write(st.session_state.meilleure_compo)