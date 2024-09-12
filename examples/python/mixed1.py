#    ------ BEGIN LICENSE ATTRIBUTION ------
#    
#    Portions of this file have been appropriated or derived from the following project(s) and therefore require attribution to the original licenses and authors.
#    
#    Project: https://www.faceswap.dev
#    Release: https://github.com/deepfakes/faceswap/releases/tag/v2.0.0
#    Source File: faceswap.py
#    
#    Copyrights:
#      copyright (c) <year>  <name of author>
#      copyright (c) 2014 adrian rosebrock, http://www.pyimagesearch.com
#      copyright (c) 2017 fariz rahman
#      (c) > 127 for c in fnt])])
#      copyright (c) 2007 free software foundation, inc. <http://fsf.org/>
#      copyright (c) 2016 kaipeng zhang
#      copyright (c) 2018 openai (http://openai.com)
#      copyright treaty adopted on 20 december 1996, or
#      copyright (c) 2011, the scikit-image team all rights reserved
#      copyright (c) 2017 yin guobing
#    
#    Licenses:
#      GNU General Public License v3.0 only
#      SPDXId: GPL-3.0-only
#    
#    Auto-attribution by Threatrix, Inc.
#    
#    ------ END LICENSE ATTRIBUTION ------
# https://github.com/healthchecks/healthchecks/blob/v1.15.0/hc/api/management/commands/prunepingsslow.py
class Command(BaseCommand):
    help = """Prune pings based on limits in user profiles.
    This command prunes each check individually. So it does the work
    in small chunks instead of a few big SQL queries like the `prunepings`
    command. It is appropriate for initial pruning of the potentially
    huge api_ping table.
    """

    def handle(self, *args, **options):
        # Create any missing user profiles
        for user in User.objects.filter(profile=None):
            Profile.objects.get_or_create(user_id=user.id)

        checks = Check.objects
        checks = checks.annotate(limit=F("project__owner__profile__ping_log_limit"))

        for check in checks:
            q = Ping.objects.filter(owner_id=check.id)
            q = q.filter(n__lte=check.n_pings - check.limit)
            q = q.filter(n__gt=0)
            n_pruned, _ = q.delete()

            self.stdout.write(
                "Pruned %d pings for check %s (%s)" % (n_pruned, check.id, check.name)
            )

        return "Done!"

# https://github.com/deepfakes/faceswap/blob/v1.0.0/faceswap.py
def _main():
    """ The main entry point into Faceswap.
    - Generates the config files, if they don't pre-exist.
    - Compiles the :class:`~lib.cli.args.FullHelpArgumentParser` objects for each section of
      Faceswap.
    - Sets the default values and launches the relevant script.
    - Outputs help if invalid parameters are provided.
    """
    generate_configs()

    subparser = _PARSER.add_subparsers()
    args.ExtractArgs(subparser, "extract", "Extract the faces from pictures")
    args.TrainArgs(subparser, "train", "This command trains the model for the two faces A and B")
    args.ConvertArgs(subparser,
                     "convert",
                     "Convert a source image to a new one with the face swapped")
    args.GuiArgs(subparser, "gui", "Launch the Faceswap Graphical User Interface")
    _PARSER.set_defaults(func=_bad_args)
    arguments = _PARSER.parse_args()
    arguments.func(arguments)


if __name__ == "__main__":
    _main()
