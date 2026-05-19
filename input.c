void clearInputBuffer() {
    // proper input handling
    int c;
    while ((c = getchar()) != '\n' &&
           c != EOF); // clear leftover character in buffer
}

char getChoice(
    const char *prompt,
    const char *valid
) {

    char input[100];

    while (1) {

        printf("%s", prompt);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
        // fully read the input
            continue;
        }

        if (input[1] != '\n') {
        // if more than one character in input
            printf(
                "Enter only one character.\n"
            );

            continue;
        }

        char c = input[0];
        // check character validity
        for (int i = 0;
             valid[i] != '\0';
             i++) {

            if (c == valid[i])
                return c;
        }

        printf("Invalid input.\n");
    }
}
