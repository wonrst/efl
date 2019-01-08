#define CODE_ANALYSIS

using System;
using System.Diagnostics.CodeAnalysis;

namespace TestSuite {

[SuppressMessage("Gendarme.Rules.Portability", "DoNotHardcodePathsRule")]
public static class TestModel {

    public class VeggieViewModel
    {
        public string Name { get; set; }
        public string Type { get; set; }
        public string Image { get; set; }        
    }
    
    public static void reflection_test ()
    {
        Efl.MonoModel<VeggieViewModel> veggies = new Efl.MonoModel<VeggieViewModel>();
        veggies.Add (new VeggieViewModel{ Name="Tomato", Type="Fruit", Image="tomato.png"});
        veggies.Add (new VeggieViewModel{ Name="Romaine Lettuce", Type="Vegetable", Image="lettuce.png"});
        veggies.Add (new VeggieViewModel{ Name="Zucchini", Type="Vegetable", Image="zucchini.png"});

        Console.WriteLine ("end of test");
    }

    internal static async System.Threading.Tasks.Task easy_model_extraction_async()
    {
        Efl.MonoModel<VeggieViewModel> veggies = new Efl.MonoModel<VeggieViewModel>();
        veggies.Add (new VeggieViewModel{ Name="Tomato", Type="Fruit", Image="tomato.png"});
        veggies.Add (new VeggieViewModel{ Name="Romaine Lettuce", Type="Vegetable", Image="lettuce.png"});
        veggies.Add (new VeggieViewModel{ Name="Zucchini", Type="Vegetable", Image="zucchini.png"});

        veggies["selected"] = true;
        Console.WriteLine ("veggies size model {0}", veggies.GetChildrenCount());

        var model = new Efl.Mono2Model<VeggieViewModel>(veggies);

        Console.WriteLine ("Waiting");
        var veggie1 = await model.GetItemAsync(1);
        Console.WriteLine ("Waited");
                
        Console.WriteLine ("end of test veggie Name |" + veggie1.Name + "|");
    }
    
    public static void easy_model_extraction ()
    {
        System.Threading.Tasks.Task task = easy_model_extraction_async();
        //t.Start();
        task.Wait();
    }
    
}

}
