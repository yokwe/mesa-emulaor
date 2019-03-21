package mesa.courier.compiler;

public class ConstantString extends Constant {
	public final String value;
	protected ConstantString(String value_) {
		super(Kind.STRING);
		// replace "" with "
		value = value_.replace("\"\"", "\\\"");
		
		// sanity check
		if (value == null)
			throw new CompilerException(String.format("value is null"));
		if (65536 <= value.length())
			throw new CompilerException(String.format("Length of value is too long. length = %d", value.length()));
	}
	@Override
	public String toString() {
		return String.format("\"%s\"", value);
	}
}
