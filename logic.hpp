#ifndef LOGIC
#define LOGIC

#include "declarations.hpp"

using namespace std;
using namespace nlohmann;
using namespace sql;

bool autentificare_cont(void)
{
    clear_screen();

    char *_HN = (char *)malloc(MAXL * sizeof(char) + 1),
         *_UN = (char *)malloc(MAXL * sizeof(char) + 1),
         *_P = (char *)malloc(MAXL * sizeof(char) + 1);

    cout << "\n\n"
         << setw(10) << " "
         << "CONECTARE BAZA DE DATE\n";
    underline(40, true);

    cout << setw(4) << " "
         << "SERVER NAME: ";
    cin >> _HN;
    cout << setw(4) << " "
         << "USERNAME: ";
    cin >> _UN;
    cout << setw(4) << " "
         << "PASSWORD: ";
    cin >> _P;

    if (strlen(_HN) > MAX_SIZE || strlen(_UN) > MAX_SIZE || strlen(_P) > MAX_SIZE)
        return EXIT_FAILURE;

    autentificare.introducere_date(_HN, _UN, _P);

    clear_screen();

    if (accesareDate() == EXIT_FAILURE)
    {
        getch();
        free(_HN);
        free(_UN);
        free(_P);
        return autentificare_cont();
    }

    free(_HN);
    free(_UN);
    free(_P);

    return EXIT_SUCCESS;
}

bool _init_(void)
{
    ifstream file_json("distante_orase.json");
    if (!file_json.is_open())
    {
        cerr << "Eroare\n";
        return EXIT_FAILURE;
    }
    else
    {
        json data;
        try
        {
            file_json >> data;
        }
        catch (json::parse_error &e)
        {
            std::cerr << e.what() << '\n';
            return EXIT_FAILURE;
        }

        for (json::iterator i = data.begin(); i != data.end(); i++)
        {
            char *pereche_orase = (char *)malloc(MAXL * sizeof(char) + 1);
            char *oras1 = (char *)malloc(MAXL * sizeof(char) + 1);
            char *oras2 = (char *)malloc(MAXL * sizeof(char) + 1);

            strcpy(pereche_orase, i.key().c_str());

            char *ptr = strtok(pereche_orase, "_");

            int ID_Oras1 = 0, ID_Oras2 = 0;
            bool nume_oras = true;

            if (ptr != NULL)
            {
                strcpy(oras1, ptr);
                ptr = strtok(NULL, "_");
            }

            if (ptr != NULL)
                strcpy(oras2, ptr);

            ORAS::NOD_ORAS *p = oras.getHead();
            bool exit = false;
            while (p != nullptr && !exit)
            {
                if (strcasecmp(p->denumire_oras, oras1) == 0)
                {
                    exit = true;
                    ID_Oras1 = stoi(p->ID_Oras);
                }
                p = p->next;
            }

            exit = false;
            p = oras.getHead();

            while (p != nullptr && !exit)
            {
                if (strcasecmp(p->denumire_oras, oras2) == 0)
                {
                    exit = true;
                    ID_Oras2 = stoi(p->ID_Oras);
                }
                p = p->next;
            }

            matrice_drum[ID_Oras1][ID_Oras2] = matrice_drum[ID_Oras2][ID_Oras1] = i.value();

            free(pereche_orase);
            free(oras1);
            free(oras2);
        }
        return EXIT_SUCCESS;
    }
}

void afisare_date_tabel_oras(void)
{
    clear_screen();

    cout << "\n\n";
    cout << setw(5) << " "
         << "┌──────────────┐\n";
    cout << setw(6) << " "
         << "TABEL-DEPOZIT\n";
    cout << setw(5) << " "
         << "└──────────────┘\n\n";

    cout << setw(5) << " "
         << "ID_Oras"
         << setw(5) << " "
         << "Denumire_Oras"
         << setw(5) << " "
         << "Tip_Depozit"
         << setw(5) << " "
         << "Latitudine"
         << setw(5) << " "
         << "Longitudine\n";
    underline(80, true);

    int cmax = -1;
    for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
    {
        if (strlen(date_oras->denumire_oras) > cmax)
            cmax = strlen(date_oras->denumire_oras);
    }

    for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
    {
        cout << setw(7) << " " << date_oras->ID_Oras << setw(10) << " " << date_oras->denumire_oras
             << setw(cmax - strlen(date_oras->denumire_oras) + 20) << " " << date_oras->tip_depozit
             << setw(11 - strlen(date_oras->tip_depozit) + 4) << " " << date_oras->latitudine << "\u00B0" << setw(8)
             << " " << date_oras->longitudine << "\u00B0\n";
    }

    underline(80, true);
}

void afisare_date_tabel_depozit(void)
{
    DEPOZIT::NOD_DEPOZIT *ptr = depozit.getHead();
    while (ptr != nullptr)
    {
        if (ptr->Cantitate_Produs == 0)
        {
            cout << "ID_Produs: " << ptr->ID_Produs << ", ";
            cout << "Cantitate_Produs: " << ptr->Cantitate_Produs << ", ";
            cout << "ID_oras: " << ptr->ID_Oras << endl;
        }
        ptr = ptr->next;
    }
}

void afisare_date_tabel_produs(void)
{
    clear_screen();

    cout << "\n\n";
    cout << setw(5) << " "
         << "┌──────────────┐\n";
    cout << setw(6) << " "
         << "TABEL-PRODUS\n";
    cout << setw(5) << " "
         << "└──────────────┘\n\n";

    cout << setw(5) << " "
         << "ID_Produs"
         << setw(10) << " "
         << "Denumire_Produs"
         << setw(35) << " "
         << "Categorie_Produs"
         << setw(13) << " "
         << "Pret_Produs\n";

    underline(115, true);

    int cmax_dp = 0, cmax_cp = 0;

    for (DETALII_PRODUS::NOD_DETALII_PRODUS *date_produs = produs.getHead(); date_produs != nullptr; date_produs = date_produs->next)
    {
        if (strlen(date_produs->Denumire_Produs) > cmax_dp)
            cmax_dp = strlen(date_produs->Denumire_Produs);
        if (strlen(date_produs->Categorie_Produs) > cmax_cp)
            cmax_cp = strlen(date_produs->Categorie_Produs);
    }

    for (DETALII_PRODUS::NOD_DETALII_PRODUS *date_produs = produs.getHead(); date_produs != nullptr; date_produs = date_produs->next)
    {
        cout << setw(7) << " " << date_produs->ID_Produs << setw(15) << " " << date_produs->Denumire_Produs
             << setw(cmax_dp - strlen(date_produs->Denumire_Produs) + 10) << " " << date_produs->Categorie_Produs
             << setw(cmax_cp - strlen(date_produs->Categorie_Produs) + 5) << " " << date_produs->pret_produs << "\n";
    }

    underline(115, true);
}

void sortare_date_depozit(void)
{
    bool vsort = true;

    DEPOZIT::NOD_DEPOZIT *l_date_depozit = nullptr;
    if (depozit.getHead() == nullptr)
        return;
    do
    {
        vsort = true;
        DEPOZIT::NOD_DEPOZIT *ptr = depozit.getHead();
        while (ptr->next != nullptr)
        {
            int _ID1 = stoi(ptr->ID_Produs), _ID2 = stoi(ptr->next->ID_Produs);
            if (_ID1 > _ID2)
            {
                swap(ptr->ID_Produs, ptr->next->ID_Produs);
                swap(ptr->ID_Oras, ptr->next->ID_Oras);
                swap(ptr->Cantitate_Produs, ptr->next->Cantitate_Produs);
                vsort = false;
            }
            ptr = ptr->next;
        }
        l_date_depozit = ptr;
    } while (!vsort);
}

void sortare_date_produs(void)
{
    bool vsort = true;

    DETALII_PRODUS::NOD_DETALII_PRODUS *l_date_produs = nullptr;
    if (produs.getHead() == nullptr)
        return;
    do
    {
        vsort = true;
        DETALII_PRODUS::NOD_DETALII_PRODUS *ptr = produs.getHead();
        while (ptr->next != nullptr)
        {
            int _ID1 = stoi(ptr->ID_Produs), _ID2 = stoi(ptr->next->ID_Produs);
            if (_ID1 > _ID2)
            {
                swap(ptr->ID_Produs, ptr->next->ID_Produs);
                swap(ptr->Categorie_Produs, ptr->next->Categorie_Produs);
                swap(ptr->Denumire_Produs, ptr->next->Denumire_Produs);
                swap(ptr->pret_produs, ptr->next->pret_produs);
                vsort = false;
            }
            ptr = ptr->next;
        }
        l_date_produs = ptr;
    } while (!vsort);
}

void sortare_date_oras(void)
{
    bool vsort = true;

    ORAS::NOD_ORAS *l_date_oras = nullptr;
    if (oras.getHead() == nullptr)
        return;
    do
    {
        vsort = true;
        ORAS::NOD_ORAS *ptr = oras.getHead();
        while (ptr->next != nullptr)
        {
            int _ID1 = stoi(ptr->ID_Oras), _ID2 = stoi(ptr->next->ID_Oras);
            if (_ID1 > _ID2)
            {
                swap(ptr->ID_Oras, ptr->next->ID_Oras);
                swap(ptr->denumire_oras, ptr->next->denumire_oras);
                swap(ptr->latitudine, ptr->next->latitudine);
                swap(ptr->longitudine, ptr->next->longitudine);
                vsort = false;
            }
            ptr = ptr->next;
        }
        l_date_oras = ptr;
    } while (!vsort);
}

void nr_max_caractere_den_produse(void)
{
    for (DETALII_PRODUS::NOD_DETALII_PRODUS *date_produs = produs.getHead(); date_produs != nullptr; date_produs = date_produs->next)
        if (strlen(date_produs->Denumire_Produs) > cmax_denumire_produse)
            cmax_denumire_produse = strlen(date_produs->Denumire_Produs);
}

bool verificare_orase_stoc_limitat(void)
{
    for (unsigned int i = 0; i < matrice_drum.size(); i++)
        if (orase_stoc_limitat[i])
            return true;
    return false;
}

void cautare_produse_ID(const int ID_Depozit)
{
    clear_screen();

    DETALII_PRODUS::NOD_DETALII_PRODUS *date_produs = produs.getHead();
    int cmax = 0;
    while (date_produs != nullptr)
    {
        if (strlen(date_produs->Denumire_Produs) > cmax)
            cmax = strlen(date_produs->Denumire_Produs);
        date_produs = date_produs->next;
    }

    cout << "\n\n"
         << setw(5) << " "
         << "ID_Produs" << setw(5) << " "
         << "Denumire_Produs" << setw(10) << " "
         << "Nr.Produse\n";
    underline(55, true);

    DEPOZIT::NOD_DEPOZIT *date_depozit = depozit.getHead();
    while (date_depozit != nullptr)
    {
        int tID_Depozit = stoi(date_depozit->ID_Oras);
        if (tID_Depozit == ID_Depozit)
            if (date_depozit->Cantitate_Produs < VAL_STOC_MINIM)
            {
                date_produs = produs.getHead();
                int tID_Produs = stoi(date_depozit->ID_Produs);
                while (date_produs != nullptr)
                {
                    int tsID_Produs = stoi(date_produs->ID_Produs);
                    if (tsID_Produs == tID_Produs)
                        cout << setw(7) << " [" << date_produs->ID_Produs << "]" << setw(8) << " " << date_produs->Denumire_Produs
                             << setw(cmax - strlen(date_produs->Denumire_Produs) - 11) << " ";
                    date_produs = date_produs->next;
                }
                cout << date_depozit->Cantitate_Produs << "\n";
            }
        date_depozit = date_depozit->next;
    }
    underline(55, true);
}

void depozite_conectate(int ID_Depozit)
{
    cout << "\n";
    vector<bool> temp_depozite(N, 100);
    ORAS::NOD_ORAS *date_oras = oras.getHead();
    char *t_denumire = (char *)malloc(MAXL * sizeof(char) + 1);
    int cmax = 0;

    while (date_oras != nullptr)
    {
        int _ID = stoi(date_oras->ID_Oras);
        if (_ID == ID_Depozit)
            strcpy(t_denumire, date_oras->denumire_oras);
        if (strlen(date_oras->denumire_oras) > cmax)
            cmax = strlen(date_oras->denumire_oras);
        date_oras = date_oras->next;
    }

    int contor = 0;
    for (unsigned int i = 0; i < matrice_drum.size(); i++)
        if (matrice_drum[ID_Depozit][i] == 1)
            temp_depozite[i] = true;

    for (unsigned int i = 0; i < matrice_drum.size(); i++)
        if (temp_depozite[i] == true)
        {
            date_oras = oras.getHead();
            while (date_oras != nullptr)
            {
                int _ID = stoi(date_oras->ID_Oras);
                if (_ID == i && strcasecmp(t_denumire, date_oras->denumire_oras) != 0)
                {
                    cout << setw(5) << " " << t_denumire << " -> " << date_oras->denumire_oras << setw(cmax - strlen(date_oras->denumire_oras) + 5)
                         << " " << matrice_drum[ID_Depozit][i] << "km\n";
                    break;
                }
                date_oras = date_oras->next;
            }
        }
}

void determinare_tip_depozit(void)
{
    ORAS::NOD_ORAS *date_oras = oras.getHead();
    while (date_oras != nullptr)
    {
        if (strcasecmp(date_oras->tip_depozit, "centralizat") == 0)
        {
            int ID = stoi(date_oras->ID_Oras);
            depozite_centralizate[ID] = true;
        }
        date_oras = date_oras->next;
    }
}

void cautare_orase_stoc_limitat(void)
{
    ORAS::NOD_ORAS *date_oras = oras.getHead();
    while (date_oras != nullptr)
    {
        int _ID_Oras = stoi(date_oras->ID_Oras);

        DEPOZIT::NOD_DEPOZIT *date_depozit = depozit.getHead();
        while (date_depozit != nullptr)
        {
            int _ID_Depozit = stoi(date_depozit->ID_Oras);
            if (_ID_Depozit == _ID_Oras && depozite_centralizate[_ID_Depozit] == false)
                if (date_depozit->Cantitate_Produs < VAL_STOC_MINIM)
                {
                    orase_stoc_limitat[_ID_Depozit] = true;
                    contor_orase_stoc_limitat++;
                    break;
                }
            date_depozit = date_depozit->next;
        }
        date_oras = date_oras->next;
    }
}

void cautare_orase_izolate(void)
{
    for (unsigned int i = 0; i < matrice_drum.size(); i++)
    {
        bool izolat = true;
        for (unsigned int j = 0; j < matrice_drum.size() && izolat; j++)
            if (matrice_drum[i][j] > 0)
                izolat = false;
        if (izolat)
            orase_izolate[i] = true;
    }
}

void vizualizare_status_stoc(void)
{
    clear_screen();

    cout << "\n\n"
         << setw(5) << " "
         << "Orase cu stocuri insuficiente:\n";
    underline(100, true);

    ORAS::NOD_ORAS *date_oras = oras.getHead();
    int cmax = 0, contor_linii = 0;

    while (date_oras != nullptr)
    {
        if (strlen(date_oras->denumire_oras) > cmax)
            cmax = strlen(date_oras->denumire_oras);
        date_oras = date_oras->next;
    }

    for (unsigned int i = 0; i < matrice_drum.size(); i++)
        if (orase_stoc_limitat[i])
        {
            date_oras = oras.getHead();
            while (date_oras != nullptr)
            {
                int _ID = stoi(date_oras->ID_Oras);
                if (_ID == i)
                {
                    cout << setw(5) << " [" << date_oras->ID_Oras << "] " << date_oras->denumire_oras
                         << setw(cmax - strlen(date_oras->denumire_oras) + 5) << " ";
                    contor_linii++;
                    if (contor_linii % 3 == 0)
                        cout << "\n";
                }
                date_oras = date_oras->next;
            }
        }

    cout << "\n";
    underline(100, true);

    char *t_ID_Oras = (char *)malloc(MAXL * sizeof(char) + 1);
    cout << setw(5) << " "
         << "Introduceti ID-ul orasului: ";
    cin >> t_ID_Oras;

    if (strcasecmp(t_ID_Oras, "exit") == 0)
        return;
    else
    {
        int _ID_Oras = stoi(t_ID_Oras);
        date_oras = oras.getHead();
        while (date_oras != nullptr)
        {
            int t_ID = stoi(date_oras->ID_Oras);
            if (t_ID == _ID_Oras && orase_stoc_limitat[t_ID] == true)
            {
                unsigned int sMENIU;

                do
                {
                    clear_screen();

                    cout << "\n\n"
                         << setw(5 + 1) << " [" << date_oras->ID_Oras << "] " << date_oras->denumire_oras
                         << " | Tip depozit: " << date_oras->tip_depozit << "\n";

                    underline(50, true);

                    cout << setw(5) << " "
                         << "[1] Vizualizare produse cu stoc limitat\n"
                         << setw(5) << " "
                         << "[2] Vizualizare conexiuni cu alte depozite\n"
                         << setw(5) << " "
                         << "[0] Inapoi\n";

                    underline(50, true);

                    cout << setw(5) << " "
                         << "Introduceti numarul meniului: ";
                    cin >> sMENIU;

                    switch (sMENIU)
                    {
                    case 1:
                        cautare_produse_ID(_ID_Oras);
                        getch();
                        break;
                    case 2:
                        depozite_conectate(_ID_Oras);
                        getch();
                        break;

                    default:
                        break;
                    }

                } while (sMENIU != 0);

                break;
            }
            date_oras = date_oras->next;
        }
        vizualizare_status_stoc();
    }
}

void creare_solutie_distanta(int start, vector<double> &distanta, vector<int> &distanta_minima, bool afisare, bool creare_trasee)
{
    int contor = 0;
    vector<bool> temp(matrice_drum.size(), false);
    temp.assign(orase_stoc_limitat.begin(), orase_stoc_limitat.end());

    for (unsigned int i = 0; i < matrice_drum.size(); i++)
    {
        contor = 0;
        if (i != start)
        {
            if (afisare)
                cout << "Shortest distance from " << start << " to " << i << " is " << distanta[i] << ". traseu: ";
            vector<int> traseu;
            int nod = i;

            while (nod != -1)
            {
                traseu.push_back(nod);
                nod = distanta_minima[nod];
            }

            int dimensiune_vector_traseu = traseu.size();

            reverse(traseu.begin(), traseu.end());

            for (unsigned int j = 0; j < dimensiune_vector_traseu; j++)
            {
                if (orase_stoc_limitat[traseu[j]])
                {
                    contor++;
                    temp[traseu[j]] = false;
                }
            }

            if (creare_trasee)
                _traseu.inserareDateTraseu(start, i, distanta[i], traseu);

            if (afisare)
                for (unsigned int j = 0; j < traseu.size(); j++)
                    cout << traseu[j] << " ";

            if (contor > nr_maxim_orase_parcurse)
            {
                nr_maxim_orase_parcurse = contor;
                _verificare_orase_parcurse.assign(traseu.begin(), traseu.end());
            }

            if (afisare)
                cout << "\n";
        }
    }
}

void dijkstra(int start, vector<double> &distanta, vector<int> &distanta_minima)
{
    vector<bool> visited(matrice_drum.size(), false);
    distanta[start] = 0.0;

    for (unsigned int i = 0; i < matrice_drum.size(); i++)
    {
        int min_index = 0;
        double min_dist = numeric_limits<double>::infinity();

        for (unsigned int j = 0; j < matrice_drum.size(); j++)
            if (!visited[j] && distanta[j] < min_dist)
            {
                min_index = j;
                min_dist = distanta[j];
            }

        visited[min_index] = true;

        for (unsigned int j = 0; j < matrice_drum.size(); j++)
        {
            double distanta_noua = distanta[min_index] + matrice_drum[min_index][j];

            if (!visited[j] && matrice_drum[min_index][j] > 0 && distanta_noua < distanta[j])
            {
                distanta[j] = distanta_noua;
                distanta_minima[j] = min_index;
            }
        }
    }
}

void afisare_depozite_centralizare(void)
{
    cout << "\n"
         << setw(5) << " "
         << "Depozite centralizate\n";
    underline(40, true);

    for (unsigned int i = 0; i < matrice_drum.size(); i++)
    {
        if (depozite_centralizate[i])
            for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
            {
                int ID = stoi(date_oras->ID_Oras);
                if (ID == i)
                {
                    cout << setw(5 + 1) << " [" << ID << "] " << date_oras->denumire_oras << endl;
                    break;
                }
            }
    }

    underline(40, true);
}

void afisare_trasee_optime(const int _ID, const int vStart)
{
    bool gasit = false;

    if (orase_izolate[_ID] == true)
    {
        cout << "\n"
             << setw(5) << " "
             << "Nu exista traseul cu acest ID. Depozit izolat...";
        return;
    }

    for (TRASEU::NOD_TRASEU *date_traseu = _traseu.getHead(); date_traseu != nullptr; date_traseu = date_traseu->next)
    {
        if (date_traseu->destinatie == _ID && date_traseu->start == vStart)
        {
            gasit = true;
            cout << "\n"
                 << setw(5) << " "
                 << "Distanta: ";
            cout << date_traseu->distanta << "km\n"
                 << setw(5) << " ";

            for (unsigned int i = 0; i < date_traseu->traseu.size(); i++)
                for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
                {
                    int ID = stoi(date_oras->ID_Oras);

                    if (ID == date_traseu->traseu[i])
                    {
                        cout << date_oras->denumire_oras;
                        // corectare afisare '->'
                        if (i != date_traseu->traseu.size() - 1)
                            cout << " --> ";
                        break;
                    }
                }
            break;
        }
    }

    if (!gasit)
    {
        cout << "\n"
             << setw(5) << " "
             << "Nu exista traseul cu acest ID!";
        return;
    }
}

void afisare_optiuni_trasee_optime(const int vStart)
{
    clear_screen();

    cout << "\n\n"
         << setw(5) << " "
         << "SCRIE 'exit' PENTRU A TE INTOARCE...\n";

    underline(45, true);

    char *oras_start = (char *)malloc(MAXL * sizeof(char) + 1);

    for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
    {
        int ID = stoi(date_oras->ID_Oras);
        if (ID == vStart)
        {
            strcpy(oras_start, date_oras->denumire_oras);
            break;
        }
    }

    for (TRASEU::NOD_TRASEU *date_traseu = _traseu.getHead(); date_traseu != nullptr; date_traseu = date_traseu->next)
    {
        if (date_traseu->start == vStart)
        {
            for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
            {
                int ID = stoi(date_oras->ID_Oras);
                if (ID == date_traseu->destinatie)
                {
                    cout << setw(5 + 1) << " [" << date_oras->ID_Oras << "] ";
                    cout << oras_start << " -> " << date_oras->denumire_oras << endl;
                    break;
                }
            }
        }
    }

    underline(45, true);

    char *_ID = (char *)malloc(MAXL * sizeof(char) + 1);
    cout << setw(5) << " "
         << "Introduceti ID-ul corespunzator: ";
    cin >> _ID;
    if (strcasecmp(_ID, "exit") == 0)
        return;
    else
    {
        int _ID_temp = stoi(_ID);
        afisare_trasee_optime(_ID_temp, vStart);
        getch();
        afisare_optiuni_trasee_optime(vStart);
    }
}

void sistem_aprovizionare_independent(void)
{
    if (!trasee)
    {
        for (unsigned int i = 0; i < matrice_drum.size(); i++)
        {
            vector<int> distanta_minima(N, -1);
            vector<double> distanta(N, numeric_limits<double>::infinity());

            if (depozite_centralizate[i])
            {
                dijkstra(i, distanta, distanta_minima);
                if (!trasee)
                    creare_solutie_distanta(i, distanta, distanta_minima, false, true);
                else
                {
                    trasee = true;
                    creare_solutie_distanta(i, distanta, distanta_minima, false, false);
                }
            }
            distanta_minima.clear();
            distanta.clear();
        }
        trasee = true;
    }

    clear_screen();

    cout << "\n\n"
         << setw(5) << " "
         << "SCRIE 'exit' PENTRU A TE INTOARCE...\n";

    afisare_depozite_centralizare();

    char *_ID = (char *)malloc(MAXL * sizeof(char) + 1);
    cout << setw(5) << " "
         << "Introduceti ID-ul corespunzator: ";
    cin >> _ID;
    if (strcasecmp(_ID, "exit") == 0)
        return;
    else
    {
        int _ID_temp = stoi(_ID);

        if (depozite_centralizate[_ID_temp])
        {
            afisare_optiuni_trasee_optime(_ID_temp);
            sistem_aprovizionare_independent();
        }
        else
        {
            cout << "\n"
                 << setw(5) << " "
                 << "Nu exista depozitul central cu acest ID...";
            getch();
            sistem_aprovizionare_independent();
        }
    }
}

void afisare_depozite_izolate(void)
{
    clear_screen();

    cout << "\n\n";
    cout << setw(5) << " "
         << "┌────────────────┐\n";
    cout << setw(6) << " "
         << "DEPOZITE IZOLATE\n";
    cout << setw(5) << " "
         << "└────────────────┘\n\n";

    cout << setw(5) << " "
         << "ID_Oras"
         << setw(5) << " "
         << "Denumire_Oras"
         << setw(5) << " "
         << "Tip_Depozit"
         << setw(5) << " "
         << "Latitudine"
         << setw(5) << " "
         << "Longitudine\n";

    underline(75, true);

    int cmax = -1;
    for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
    {
        if (strlen(date_oras->denumire_oras) > cmax)
            cmax = strlen(date_oras->denumire_oras);
    }

    bool gasit = false;
    for (unsigned int i = 0; i < orase_izolate.size(); i++)
        if (orase_izolate[i] == true)
        {
            gasit = true;
            for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
            {
                int ID = stoi(date_oras->ID_Oras);
                if (ID == i)
                {
                    cout << setw(7) << " " << date_oras->ID_Oras << setw(12) << " " << date_oras->denumire_oras
                         << setw(cmax - strlen(date_oras->denumire_oras) + 18) << " " << date_oras->tip_depozit
                         << setw(11 - strlen(date_oras->tip_depozit) + 4) << " " << date_oras->latitudine << "\u00B0" << setw(8)
                         << " " << date_oras->longitudine << "\u00B0\n";
                    break;
                }
            }
        }

    underline(75, true);

    if (!gasit)
    {
        cout << setw(5) << " "
             << "Nu exista depozite izolate!";
        return;
    }
}

void afisare_depozite_unic_drum(void)
{
    clear_screen();

    cout << "\n\n";
    cout << setw(5) << " "
         << "┌─────────────────────────────┐\n";
    cout << setw(6) << " "
         << "DEPOZITE CU O UNICA CONEXIUNE\n";
    cout << setw(5) << " "
         << "└─────────────────────────────┘\n\n";

    cout << setw(5) << " "
         << "ID_Oras"
         << setw(5) << " "
         << "Denumire_Oras"
         << setw(5) << " "
         << "Tip_Depozit"
         << setw(5) << " "
         << "Latitudine"
         << setw(5) << " "
         << "Longitudine\n";

    underline(75, true);

    bool gasit = false;
    for (unsigned int i = 0; i < matrice_drum.size(); i++)
    {
        int contor = 0;
        for (unsigned int j = 0; j < matrice_drum.size(); j++)
            if (matrice_drum[i][j] != 0)
            {
                contor++;
                if (contor >= 2)
                    break;
            }
        if (contor == 1)
        {
            gasit = true;
            orase_conexiune_unica[i] = true;
        }
    }

    if (gasit)
    {
        int cmax = -1;
        for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
        {
            if (strlen(date_oras->denumire_oras) > cmax)
                cmax = strlen(date_oras->denumire_oras);
        }

        for (unsigned int i = 0; i < orase_conexiune_unica.size(); i++)
        {
            if (orase_conexiune_unica[i])
            {
                for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
                {
                    int ID = stoi(date_oras->ID_Oras);
                    if (ID == i)
                    {
                        cout << setw(7) << " " << date_oras->ID_Oras << setw(10) << " " << date_oras->denumire_oras
                             << setw(cmax - strlen(date_oras->denumire_oras) + 20) << " " << date_oras->tip_depozit
                             << setw(11 - strlen(date_oras->tip_depozit) + 4) << " " << date_oras->latitudine << "\u00B0" << setw(8)
                             << " " << date_oras->longitudine << "\u00B0\n";
                        break;
                    }
                }
            }
        }
        underline(75, true);
    }
    else
    {
        cout << setw(5) << " "
             << "Nu exista depozite cu o unica conexiune!";
        return;
    }
}

void init_stiva_hc(void)
{
    stiva[contor_stiva] = -1;
}

bool succesor_hc(void)
{
    if (stiva[contor_stiva] < N - 1)
    {
        stiva[contor_stiva]++;
        return true;
    }
    return false;
}

bool solutie_hc(void)
{
    if (contor_stiva == N)
        return true;
    return false;
}

bool valid_hc(void)
{
    for (unsigned int i = 1; i < contor_stiva; i++)
        if (stiva[contor_stiva] == stiva[i])
            return false;

    if (contor_stiva > 1)
        if (matrice_drum[stiva[contor_stiva]][stiva[contor_stiva - 1]] == 0)
            return false;

    if (contor_stiva > 1)
        if (depozite_centralizate[stiva[1]] == false)
            return false;

    return true;
}

void determinare_ciclu_hc_minim(void)
{
    double cost_curent = 0.0;
    for (unsigned int i = 1; i <= contor_stiva; i++)
        cost_curent += stiva[i];
    if (cost_curent < cost_minim_TSP)
    {
        cost_minim_TSP = cost_curent;
        traseu_minim_TSP.assign(stiva.begin(), stiva.end());
        contor_traseu_TSP = contor_stiva;
    }
}

void back_hc(void)
{
    contor_stiva = 1;
    init_stiva_hc();
    while (contor_stiva > 0)
    {
        int vSuccesor, vValid;
        do
        {
            vSuccesor = succesor_hc();
            if (vSuccesor == 1)
                vValid = valid_hc();
        } while (vSuccesor == 1 && vValid == 0);
        if (vSuccesor == 1)
        {
            if (solutie_hc() == 1)
                determinare_ciclu_hc_minim();
            else
            {
                contor_stiva++;
                init_stiva_hc();
            }
        }
        else
            contor_stiva--;
    }
}

void init_stiva_ac(void)
{
    stiva[contor_stiva] = -1;
}

bool succesor_ac(void)
{
    if (stiva[contor_stiva] < N - 1)
    {
        stiva[contor_stiva]++;
        return true;
    }
    return false;
}

bool solutie_ac(void)
{
    if (contor_stiva == N + 1)
        return true;
    return false;
}

bool valid_ac(void)
{
    if (contor_stiva == N + 1)
    {
        for (unsigned int i = 0; i < matrice_drum.size(); i++)
        {
            bool gasit = false;
            if (orase_stoc_limitat[i] == true && !orase_izolate[i])
            {
                for (unsigned int j = 1; j <= contor_stiva; j++)
                {
                    if (stiva[j] == i)
                    {
                        gasit = true;
                        break;
                    }
                }

                if (!gasit)
                    return false;
            }
        }
    }

    if (contor_stiva > 1)
        if (matrice_drum[stiva[contor_stiva]][stiva[contor_stiva - 1]] == 0)
            return false;

    if (contor_stiva > 1)
        if (depozite_centralizate[stiva[1]] == false)
            return false;

    return true;
}

void determinare_traseu_minim(void)
{
    double suma_dist = 0;
    for (int i = 1; i < contor_stiva; i++)
        suma_dist += matrice_drum[stiva[i]][stiva[i + 1]];

    if (suma_dist < cost_minim_TSP)
    {
        cost_minim_TSP = suma_dist;
        for (int i = 1; i <= contor_stiva; i++)
        {
            traseu_minim_TSP[i] = stiva[i];
            contor_traseu_TSP = contor_stiva;
        }
    }
}

void back_ac(void)
{
    int vSuccesor, vValid;
    contor_stiva = 1;
    init_stiva_ac();
    while (contor_stiva > 0)
    {
        do
        {
            vSuccesor = succesor_ac();
            if (vSuccesor == 1)
                vValid = valid_ac();
        } while (vSuccesor == 1 && vValid == 0);
        if (vSuccesor == 1)
            if (solutie_ac() == 1)
                determinare_traseu_minim();
            else
            {
                contor_stiva++;
                init_stiva_ac();
            }
        else
            contor_stiva--;
    }
}

void TSP(void)
{
    bool izolat = false;
    for (unsigned int i = 0; i < matrice_drum.size(); i++)
        if (orase_izolate[i] == true)
        {
            izolat = true;
            break;
        }

    if (!izolat)
    {
        back_hc();
        if (!traseu_minim_TSP.empty())
        {
            cout << cost_minim_TSP << "\n";
            for (unsigned int i = 1; i <= contor_traseu_TSP; i++)
                cout << traseu_minim_TSP[i] << " ";
        }
        else
        {
            traseu_minim_TSP.clear();
            stiva.clear();
            back_ac();
            if (!traseu_minim_TSP.empty())
            {
                cout << cost_minim_TSP << "\n";
                for (unsigned int i = 1; i <= contor_traseu_TSP; i++)
                    cout << traseu_minim_TSP[i] << " ";
            }
            else
                cout << "Toate depozitele sunt izolate!";
        }
    }
    else
    {
        back_ac();
        cout << "\n\n";
        if (!traseu_minim_TSP.empty())
        {
            cout << setw(5) << " "
                 << "Lungime traseu: " << cost_minim_TSP << "km\n"
                 << setw(5) << " ";
            for (unsigned int i = 1; i <= contor_traseu_TSP; i++)
            {
                for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
                {
                    int ID = stoi(date_oras->ID_Oras);
                    if (ID == traseu_minim_TSP[i])
                    {
                        cout << date_oras->denumire_oras;
                        if (i < contor_traseu_TSP)
                            cout << " --> ";
                        break;
                    }
                }
            }
        }
        else
            cout << "Toate depozitele sunt izolate!";
    }
}

void produse_transport_TSP(void)
{
    for (DETALII_PRODUS::NOD_DETALII_PRODUS *date_produs = produs.getHead(); date_produs != nullptr; date_produs = date_produs->next)
    {
        int cantitate = 0;
        int ID_PRODUS = stoi(date_produs->ID_Produs);
        for (DEPOZIT::NOD_DEPOZIT *date_depozit = depozit.getHead(); date_depozit != nullptr; date_depozit = date_depozit->next)
        {
            int ID_DEPOZIT = stoi(date_depozit->ID_Oras);
            if (orase_stoc_limitat[ID_DEPOZIT] == true && orase_izolate[ID_DEPOZIT] == false && depozite_centralizate[ID_DEPOZIT] == false)
            {
                int ID_PRODUS_DEPOZIT = stoi(date_depozit->ID_Produs);
                if (ID_PRODUS == ID_PRODUS_DEPOZIT)
                    cantitate += VAL_STOC_MAXIM - date_depozit->Cantitate_Produs;
            }
        }
        aprovizionare.inserareDateAprovizionare(date_produs->ID_Produs, cantitate);
    }
}

void pagina_principala_TSP(void)
{
    clear_screen();

    distanta_parcursa = 0.0;

    cout << "\n";
    if (!traseu_minim_TSP.empty())
    {
        cout << setw(5) << " "
             << "Lungime traseu: " << cost_minim_TSP << "km\n"
             << setw(5) << " ";
        for (unsigned int i = 1; i <= contor_traseu_TSP; i++)
        {
            for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
            {
                int ID = stoi(date_oras->ID_Oras);

                if (i == 1)
                    if (ID == traseu_minim_TSP[1])
                        cout << "\033[4m"
                             << "\033[1m" << date_oras->denumire_oras << "\033[0m"
                             << " --> ";

                if (ID == traseu_minim_TSP[i] && i != 1)
                {
                    cout << date_oras->denumire_oras;
                    if (i < contor_traseu_TSP)
                        cout << " --> ";
                    break;
                }
            }
        }
    }
    cout << "\n";
    underline(190, false);

    cout << "\n";
    for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
    {
        int ID = stoi(date_oras->ID_Oras);
        if (ID == traseu_minim_TSP[1])
        {
            cout << setw(5) << " "
                 << "┌───────────────────────┐\n";
            cout << setw(8) << " [" << date_oras->ID_Oras << "] " << date_oras->denumire_oras << " - START\n";
            cout << setw(5) << " "
                 << "└───────────────────────┘";
            break;
        }
    }

    cout << setw(120) << " "
         << "\033[1m"
         << "Distanta parcursa: "
         << "\033[0m" << distanta_parcursa << "km\n";

    cout << "\n\n"
         << setw(5) << " "
         << "ID" << setw(7) << " "
         << "Den.Produs" << setw(15) << " "
         << "Cnt.transport_totala\n";
    underline(65, true);

    for (APROVIZIONARE::NOD_APROVIZIONARE *date_aprovizionare = aprovizionare.getHead(); date_aprovizionare != nullptr; date_aprovizionare = date_aprovizionare->next)
    {
        int ID_AP = stoi(date_aprovizionare->ID_Produs);
        cout << setw(5 + 1) << " [" << date_aprovizionare->ID_Produs << "] " << setw(5) << " ";
        for (DETALII_PRODUS::NOD_DETALII_PRODUS *date_produs = produs.getHead(); date_produs != nullptr; date_produs = date_produs->next)
        {
            int ID_P = stoi(date_produs->ID_Produs);
            if (ID_P == ID_AP)
            {
                cout << date_produs->Denumire_Produs << setw(cmax_denumire_produse - strlen(date_produs->Denumire_Produs) + 5) << " ";
                break;
            }
        }
        cout << date_aprovizionare->cantitate_totala_necesara << " buc.\n";
    }
    underline(65, true);

    cout << "\n\n";
    underline(190, false);

    cout << setw(5) << " "
         << "[1] PREV" << setw(80) << " -" << pagina << "- " << setw(80) << " "
         << "[2] NEXT\n";

    underline(190, false);

    distanta_parcursa += matrice_drum[traseu_minim_TSP[1]][traseu_minim_TSP[2]];
}

void pagina_finala_TSP(void)
{
    clear_screen();

    cout << "\n";
    underline(35, true);
    cout << setw(5) << " "
         << "Doriti sa finalizati traseul?\n\n"
         << setw(5) << " "
         << "[D] Da"
         << setw(15) << " "
         << "[N] Nu\n";
    underline(35, true);

    char *input = (char *)malloc(MAXL * sizeof(char) + 1);
    cout << setw(5) << " "
         << "I: ";
    cin >> input;

    if (strcasecmp(input, "d") == 0)
    {
        traseu_completat = true;

        ofstream log_out;
        log_out.open("log.txt", ios::app);

        string s(1000, '-');

        if (log_out.is_open())
        {
            log_out << "LOG [" << contor_log << "]: ";
            for (unsigned int i = 1; i <= contor_traseu_TSP; i++)
            {
                for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
                {
                    int ID = stoi(date_oras->ID_Oras);
                    if (ID == traseu_minim_TSP[i])
                    {
                        log_out << date_oras->denumire_oras;
                        if (i < contor_traseu_TSP)
                            log_out << " --> ";
                        break;
                    }
                }
            }
            log_out << "\n"
                    << s << "\n";
            log_out << "Distanta parcursa: " << cost_minim_TSP << "km\n";
            log_out << "Cantitate totala transportata: " << cantitate_totala_aprovizionata << "BUC.\n";
            log_out << "Cost total: " << cost_aprovizionare_total << "RON\nEND-LOG\n";
            log_out << s << "\n\n\n";

            clear_screen();
            cout << "\n\n"
                 << setw(5) << " "
                 << "Aprovizionare completa.\n"
                 << setw(5) << " "
                 << "Mai multe detalii in fisierul log.txt...";
            getch();
            return;
        }
    }
    else if (!traseu_completat)
    {
        pagina = 1;
        pagina_principala_TSP();
    }

    free(input);
}

void pagina_stanga_TSP(void)
{
    clear_screen();

    if (pagina - 1 > 1)
    {
        pagina--;

        orase_stoc_limitat[traseu_minim_TSP[pagina]] = true;
        distanta_parcursa -= matrice_drum[traseu_minim_TSP[pagina]][traseu_minim_TSP[pagina - 1]];

        cout << "\n";
        if (!traseu_minim_TSP.empty())
        {
            cout << setw(5) << " "
                 << "Lungime traseu: " << cost_minim_TSP << "km\n"
                 << setw(5) << " ";
            for (unsigned int i = 1; i <= contor_traseu_TSP; i++)
            {
                for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
                {
                    int ID = stoi(date_oras->ID_Oras);

                    if (ID == traseu_minim_TSP[pagina] && i == pagina)
                    {
                        cout << "\033[4m"
                             << "\033[1m" << date_oras->denumire_oras << "\033[0m";
                        if (i < contor_traseu_TSP)
                            cout << " --> ";
                        break;
                    }

                    if (ID == traseu_minim_TSP[i] && i != pagina)
                    {
                        cout << date_oras->denumire_oras;
                        if (i < contor_traseu_TSP)
                            cout << " --> ";
                        break;
                    }
                }
            }
        }
        cout << "\n";
        underline(190, false);
        cout << "\n";

        for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
        {
            int ID = stoi(date_oras->ID_Oras);
            if (ID == traseu_minim_TSP[pagina])
            {
                cout << setw(5) << " "
                     << "┌───────────────────────┐\n";
                cout << setw(8) << " [" << date_oras->ID_Oras << "] " << date_oras->denumire_oras << "\n";
                cout << setw(5) << " "
                     << "└───────────────────────┘";
                break;
            }
        }

        cout << setw(120) << " "
             << "\033[1m"
             << "Distanta parcursa: "
             << "\033[0m" << distanta_parcursa << "km\n";

        cout << "\n\n"
             << setw(5) << " "
             << "ID" << setw(10) << " "
             << "Den.Produs" << setw(15) << " "
             << "Cnt.transport/Cnt.ramasa\n";
        underline(70, true);

        if (depozite_centralizate[traseu_minim_TSP[pagina]] == false)
        {
            if (orase_stoc_limitat[traseu_minim_TSP[pagina]])
            {
                for (DETALII_PRODUS::NOD_DETALII_PRODUS *date_produs = produs.getHead(); date_produs != nullptr; date_produs = date_produs->next)
                {
                    int ID_PRODUS = stoi(date_produs->ID_Produs);
                    double cantitate_necesara = 0.0;

                    for (DEPOZIT::NOD_DEPOZIT *date_depozit = depozit.getHead(); date_depozit != nullptr; date_depozit = date_depozit->next)
                    {
                        int ID_PRODUS_DEPOZIT = stoi(date_depozit->ID_Produs), ID_DEPOZIT = stoi(date_depozit->ID_Oras);
                        if (ID_DEPOZIT == traseu_minim_TSP[pagina] && !orase_izolate[ID_DEPOZIT])
                        {
                            if (ID_PRODUS_DEPOZIT == ID_PRODUS)
                            {
                                cantitate_necesara += date_depozit->Cantitate_Produs;
                                cantitate_necesara = VAL_STOC_MAXIM - cantitate_necesara;
                                cost_aprovizionare_total -= cantitate_necesara * date_produs->pret_produs;
                                cantitate_totala_aprovizionata -= cantitate_necesara;

                                cout << setw(5 + 1) << " [" << date_depozit->ID_Produs << "] " << setw(8) << " " << date_produs->Denumire_Produs
                                     << setw(cmax_denumire_produse - strlen(date_produs->Denumire_Produs) + 5) << " " << cantitate_necesara << " buc. /";

                                for (APROVIZIONARE::NOD_APROVIZIONARE *date_aprovizionare = aprovizionare.getHead(); date_aprovizionare != nullptr; date_aprovizionare = date_aprovizionare->next)
                                {
                                    int ID_PRODUS_APROVIZIONARE = stoi(date_aprovizionare->ID_Produs);
                                    if (ID_PRODUS_APROVIZIONARE == ID_PRODUS)
                                    {
                                        cout << date_aprovizionare->cantitate_totala_necesara << " buc.";
                                        date_aprovizionare->cantitate_totala_necesara += cantitate_necesara;
                                        break;
                                    }
                                }
                                cout << "\n";
                            }
                        }
                    }
                }
            }
            else
                cout << setw(5) << " "
                     << "\033[1m"
                     << "Depozitul a fost aprovizionat!\n"
                     << "\033[0m";
        }
        else
        {
            cout << setw(5) << " "
                 << "\033[1m"
                 << "Depozit centralizat!\n"
                 << "\033[0m";
        }

        underline(70, true);

        cout << "\n\n";
        underline(190, false);

        cout << setw(5) << " "
             << "[1] PREV" << setw(80) << " -" << pagina << "- " << setw(80) << " "
             << "[2] NEXT\n";

        underline(190, false);
    }
    else
    {
        pagina = 1;
        pagina_principala_TSP();
    }
}

void pagina_dreapta_TSP(void)
{
    clear_screen();

    if (pagina < contor_traseu_TSP)
    {
        pagina++;

        cout << "\n";
        if (!traseu_minim_TSP.empty())
        {
            cout << setw(5) << " "
                 << "Lungime traseu: " << cost_minim_TSP << "km\n"
                 << setw(5) << " ";
            for (unsigned int i = 1; i <= contor_traseu_TSP; i++)
            {
                for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
                {
                    int ID = stoi(date_oras->ID_Oras);

                    if (ID == traseu_minim_TSP[pagina] && i == pagina)
                    {
                        cout << "\033[4m"
                             << "\033[1m" << date_oras->denumire_oras << "\033[0m";
                        if (i < contor_traseu_TSP)
                            cout << " --> ";
                        break;
                    }

                    if (ID == traseu_minim_TSP[i] && i != pagina)
                    {
                        cout << date_oras->denumire_oras;
                        if (i < contor_traseu_TSP)
                            cout << " --> ";
                        break;
                    }
                }
            }
        }
        cout << "\n";
        underline(190, false);
        cout << "\n";

        for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
        {
            int ID = stoi(date_oras->ID_Oras);
            if (ID == traseu_minim_TSP[pagina])
            {
                cout << setw(5) << " "
                     << "┌───────────────────────┐\n";
                cout << setw(8) << " [" << date_oras->ID_Oras << "] " << date_oras->denumire_oras << "\n";
                cout << setw(5) << " "
                     << "└───────────────────────┘";
                break;
            }
        }

        cout << setw(120) << " "
             << "\033[1m"
             << "Distanta parcursa: "
             << "\033[0m" << distanta_parcursa << "km\n";

        cout << "\n\n"
             << setw(5) << " "
             << "ID" << setw(10) << " "
             << "Den.Produs" << setw(15) << " "
             << "Cnt.transport/Cnt.ramasa\n";
        underline(70, true);

        if (depozite_centralizate[traseu_minim_TSP[pagina]] == false)
        {
            if (orase_stoc_limitat[traseu_minim_TSP[pagina]])
            {
                for (DETALII_PRODUS::NOD_DETALII_PRODUS *date_produs = produs.getHead(); date_produs != nullptr; date_produs = date_produs->next)
                {
                    int ID_PRODUS = stoi(date_produs->ID_Produs);
                    double cantitate_necesara = 0.0;

                    for (DEPOZIT::NOD_DEPOZIT *date_depozit = depozit.getHead(); date_depozit != nullptr; date_depozit = date_depozit->next)
                    {
                        int ID_PRODUS_DEPOZIT = stoi(date_depozit->ID_Produs), ID_DEPOZIT = stoi(date_depozit->ID_Oras);
                        if (ID_DEPOZIT == traseu_minim_TSP[pagina] && !orase_izolate[ID_DEPOZIT])
                        {
                            if (ID_PRODUS_DEPOZIT == ID_PRODUS)
                            {
                                cantitate_necesara += date_depozit->Cantitate_Produs;
                                cantitate_necesara = VAL_STOC_MAXIM - cantitate_necesara;
                                cost_aprovizionare_total += cantitate_necesara * date_produs->pret_produs;
                                cantitate_totala_aprovizionata += cantitate_necesara;

                                cout << setw(5 + 1) << " [" << date_depozit->ID_Produs << "] " << setw(8) << " " << date_produs->Denumire_Produs
                                     << setw(cmax_denumire_produse - strlen(date_produs->Denumire_Produs) + 5) << " " << cantitate_necesara << " buc. /";

                                for (APROVIZIONARE::NOD_APROVIZIONARE *date_aprovizionare = aprovizionare.getHead(); date_aprovizionare != nullptr; date_aprovizionare = date_aprovizionare->next)
                                {
                                    int ID_PRODUS_APROVIZIONARE = stoi(date_aprovizionare->ID_Produs);
                                    if (ID_PRODUS_APROVIZIONARE == ID_PRODUS)
                                    {
                                        cout << date_aprovizionare->cantitate_totala_necesara << " buc.";
                                        date_aprovizionare->cantitate_totala_necesara -= cantitate_necesara;
                                        break;
                                    }
                                }
                                cout << "\n";
                            }
                        }
                    }
                }
            }
            else
                cout << setw(5) << " "
                     << "\033[1m"
                     << "Depozitul a fost aprovizionat!\n"
                     << "\033[0m";
        }
        else
            cout << setw(5) << " "
                 << "\033[1m"
                 << "Depozit centralizat!\n"
                 << "\033[0m";

        underline(70, true);

        cout << "\n\n";
        underline(190, false);

        cout << setw(5) << " "
             << "[1] PREV" << setw(80) << " -" << pagina << "- " << setw(80) << " "
             << "[2] NEXT\n";

        underline(190, false);

        orase_stoc_limitat[traseu_minim_TSP[pagina]] = false;
        distanta_parcursa += matrice_drum[traseu_minim_TSP[pagina]][traseu_minim_TSP[pagina + 1]];
    }
    else
        pagina_finala_TSP();
}

void parcurgere_traseu_TSP(void)
{
    pagina = 1;
    cout << "\n";

    if (!traseu_completat)
    {
        if (traseu_minim_TSP[1] == -1)
        {
            clear_screen();
            cout << "\n\n"
                 << setw(5) << " "
                 << "Se calculeaza traseul cel mai optim...\n";
            sleepcp(1500);
            clear_screen();
            TSP();
        }
        else
        {
            cout << setw(5) << " "
                 << "Lungime traseu: " << cost_minim_TSP << "km\n"
                 << setw(5) << " ";
            for (unsigned int i = 1; i <= contor_traseu_TSP; i++)
            {
                for (ORAS::NOD_ORAS *date_oras = oras.getHead(); date_oras != nullptr; date_oras = date_oras->next)
                {
                    int ID = stoi(date_oras->ID_Oras);
                    if (ID == traseu_minim_TSP[i])
                    {
                        cout << date_oras->denumire_oras;
                        if (i < contor_traseu_TSP)
                            cout << " --> ";
                        break;
                    }
                }
            }
        }
    }
    else
    {
        clear_screen();
        cout << "\n\n"
             << setw(5) << " "
             << "\033[1m"
             << "Nu exista depozite cu stoc limitat/epuizat..."
             << "\033[0m";
        traseu_minim_TSP.clear();
        getch();
        return;
    }

    char *input = (char *)malloc(MAXL * sizeof(char) + 1);
    cout << "\n\n\n"
         << setw(5) << " "
         << "[S] Start: ";
    cin >> input;

    pagina_principala_TSP();

    if (strcasecmp(input, "0") == 0)
        return;
    else if (strcasecmp(input, "s") == 0)
    {
        unsigned int MENIU;

        do
        {
            cout << "\n"
                 << setw(5) << " "
                 << "Introduceti numarul meniului: ";
            cin >> MENIU;

            switch (MENIU)
            {
            case 1:
                pagina_stanga_TSP();
                break;
            case 2:
                pagina_dreapta_TSP();
                break;

            default:
                pagina_finala_TSP();
                break;
            }
        } while (MENIU != 0 && traseu_completat == false);
    }
    free(input);
}

void afisare_detalii_SpeedyGo(Connection *con)
{
    cout << "\n";
    underline(90, true);

    cout << "\033[3m"
         << setw(5) << " "
         << "Bun venit în Consola MySQL. Introduceti o interogare SQL (sau 'exit' pentru a incheia)\n"
         << "\033[0m";

    underline(90, true);

    Statement *stmt = con->createStatement();
    ResultSet *res = stmt->executeQuery("SHOW TABLES");

    cout << setw(5) << " "
         << "+" << string(22, '-') << "+" << endl;
    cout << setw(5) << " "
         << "| " << left << setw(20) << "SpeedyGo - Tabele"
         << " |" << endl;
    cout << setw(5) << " "
         << "|" << string(22, '-') << "|" << endl;

    while (res->next())
    {
        cout << setw(5) << " "
             << "| " << setw(20) << left << res->getString(1) << " |" << endl;
    }

    cout << right;

    cout << setw(5) << " "
         << "+" << string(22, '-') << "+" << endl;

    underline(90, true);

    delete stmt;
    delete res;
}

void consola_mysql(void)
{
    clear_screen();

    if (buffer)
    {
        buffer = false;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    Driver *driver;
    Connection *con;

    driver = mysql::get_mysql_driver_instance();
    con = driver->connect();
    con->setSchema("SpeedyGo");

    afisare_detalii_SpeedyGo(con);

    string interogare;

    while (true)
    {
        cout << "\n"
             << setw(5) << " "
             << "mysql> ";

        getline(cin, interogare);

        if (interogare == "exit")
        {
            buffer = true;
            break;
        }
        else if (interogare == "clear")
        {
            clear_screen();
            afisare_detalii_SpeedyGo(con);
        }
        else
        {
            try
            {
                Statement *stmt = con->createStatement();
                ResultSet *res = stmt->executeQuery(interogare);

                int cnt_coloane = res->getMetaData()->getColumnCount();
                vector<int> coloane(cnt_coloane, 0);

                while (res->next())
                {
                    for (unsigned int i = 1; i <= cnt_coloane; i++)
                    {
                        int crt_width = res->getString(i).length();
                        if (crt_width > coloane[i - 1])
                        {
                            coloane[i - 1] = crt_width;
                        }
                    }
                }

                cout << "\n";
                for (int i = 1; i <= cnt_coloane; i++)
                {
                    cout << setw(5) << " " << setw(coloane[i - 1] + 5) << res->getMetaData()->getColumnName(i) << " ";
                }
                cout << "\n";

                underline(90, true);

                res->beforeFirst();

                while (res->next())
                {
                    for (int i = 1; i <= cnt_coloane; i++)
                        cout << setw(5) << " " << setw(coloane[i - 1] + 5) << res->getString(i) << " ";
                    cout << endl;
                }

                underline(90, true);

                delete stmt;
                delete res;
            }
            catch (SQLException &e)
            {
                cout << "Error code: " << e.getErrorCode() << "\n";
                cout << "Error message: " << e.what() << "\n";
                cout << "SQLState: " << e.getSQLState() << "\n";
            }
        }
    }

    interogare.clear();
    delete con;
}

#endif