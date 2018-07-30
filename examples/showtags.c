/*
 * The purpose of this example is to demonstrate how tag information can be
 * retrieved and used by an application.
 *
 * An application can retrieve tags and their types and generate a user
 * interface from the intormation at run-time.
 */
#include <stdio.h>
#include <footag/footag.h>

#define IND "    "
#define SEPARATOR \
"--------------------------------------------------------------------------"
static void prettytagdesc(int tag, const char *crit)
{
        const struct footag_desc *desc = &FOOTAG_DESC[tag];
        int type = desc->type;
        printf(
                IND "%-11s  %-8s  %-11s  %s\n",
                desc->name,
                crit,
                FOOTAG_TYPE_STRING[type],
                desc->desc
        );
}

static void prettyheader(void)
{
        puts(IND SEPARATOR);
        printf(
                IND "%-11s  %-8s  %-11s  %s\n",
                "FOOTAG_",
                "REQ/OPT",
                "TYPE",
                "DESC"
        );
        puts(IND SEPARATOR);
}

static void showinfo(const struct footag_info *info)
{
        printf("=== %s: %s ===\n", info->tname, info->brief);
        prettyheader();

        for (int *t = info->req; *t != FOOTAG_END; t++) {
                prettytagdesc(*t, "required");
        }
        for (int *t = info->opt; *t != FOOTAG_END; t++) {
                prettytagdesc(*t, "optional");
        }
        puts("");
}

int main(void)
{
        const struct footag_info *head;

        printf("This program prints footprint types available in Footag.\n\n");

        head = footag_getinfo();
        for (const struct footag_info *info = head; info; info = info->next) {
                showinfo(info);
        }


        printf("\nThe following tags are always optional:\n");

        prettyheader();
        const int *opt = footag_getalwaysopt();
        for (const int *t = opt; *t != FOOTAG_END; t++) {
                prettytagdesc(*t, "optional");
        }
        puts("");

        return 0;
}

