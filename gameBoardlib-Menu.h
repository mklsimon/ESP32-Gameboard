extern FastLED_NeoMatrix *matrix; // Déclaration de la variable globale
extern WebControler webController; // Déclaration de la variable globale

class MatrixMenu {
public:
    int currentIndex = 0;
    MatrixMenu() {
        // Vous pouvez initialiser des éléments ici si nécessaire
        
    }

    void add_item(const std::string& label, const std::vector<std::vector<CRGB>>& icon = std::vector<std::vector<CRGB>>()) {
        items[label] = icon;        
    }

    void showItems(int currentIndex, const std::string& title = "") {
        const int matrixWidth = matrix->width();
        int yOffset = 1; // Ajustez la position verticale si nécessaire
        matrix->clear();
        int index = 0; // Pour garder une trace de l'index actuel

        if (!title.empty()) {
            // Centrez le titre si spécifié
            int titleX = (matrixWidth - title.length() * 3) / 2;
            matrix->setCursor(titleX, yOffset + 2); // Ajustez la position verticale du titre

            // Dessiner le texte du titre en jaune
            uint16_t titleColor = matrix->Color(titleMenuColor.r, titleMenuColor.g, titleMenuColor.b);
            matrix->setTextColor(titleColor);
            matrix->print(title.c_str());

            // Dessiner une ligne de soulignement sous le titre
            int underlineY = yOffset + 4; // Ajustez la position verticale de la ligne de soulignement
            //matrix->drawFastHLine(titleX, underlineY, title.length() * 4 - 1 , titleColor);

            yOffset += 4; // Ajustez l'espacement entre le titre et les éléments
        }
        for (auto it = items.begin(); it != items.end(); ++it) {
            const std::string& key = it->first; // Récupérez le nom de la clé
            const char* keyChar = key.c_str();
            
            // Affichez le nom de la clé au centre
            matrix->setCursor(5, yOffset+4);
            uint16_t textColor = matrix->Color(TextMenuColor.r, TextMenuColor.g, TextMenuColor.b);
            matrix->setTextColor(textColor);
            matrix->print(keyChar);
            const std::vector<std::vector<CRGB>>& icon = it->second;

            // Si l'élément actuel est celui correspondant à currentIndex, entourez-le
            if (index == currentIndex) {
                uint16_t borderColor = matrix->Color(borderMenuColor.r, borderMenuColor.g, borderMenuColor.b);
                matrix->drawRect(0, yOffset, matrixWidth, 7, borderColor);
            }

            for (int y = 0; y < icon.size(); y++) {
                for (int x = 0; x < icon[y].size(); x++) {
                    matrix->drawPixel(x + 1, yOffset + y + 2 , icon[y][x]);
                }
            }
            // Ajoutez un espace vertical entre les éléments
            yOffset += 6; // Ajustez l'espace vertical entre les éléments si nécessaire
            index++; // Incrémentez l'index
        }
        matrix->show();
    }

    String show(const std::string& title = "", int currentIndex=0) {
        size_t numItems = items.size();
        showItems(currentIndex,title);
        while (true) {
            if (webController.gotMessages(0)){
                String command = webController.getMessages(0);
                if ( command == "down") currentIndex++;
                else if ( command == "up") currentIndex--;
                if (currentIndex < 0 ) currentIndex = numItems - 1;
                if (currentIndex > numItems -1  ) currentIndex = 0;
                if ( command == "Y" || command == "B"|| command == "G" || command == "R") {
                    int index = 0;
                    for (auto it = items.begin(); it != items.end(); ++it) {
                        if ( index == currentIndex ){
                            std::string key = it->first; // Récupérez le nom de la clé
                            matrix->clear();
                            return String(key.c_str());
                        }
                        index++;
                    }
                }
                showItems(currentIndex,title);
                delay(100);
            }
        }   
    }
    void clear() {
        items.clear();
    }

private:
    std::map<std::string, std::vector<std::vector<CRGB>>> items;
};
